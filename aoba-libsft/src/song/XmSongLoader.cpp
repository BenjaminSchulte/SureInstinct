#include <QDebug>
#include <QtMath>
#include <QFileInfo>
#include <sft/utils/DataStream.hpp>
#include <sft/sample/Sample.hpp>
#include <sft/sample/SampleData.hpp>
#include <sft/song/XmSongLoader.hpp>
#include <sft/song/Song.hpp>
#include <sft/mml/MmlScript.hpp>

using namespace Sft;

namespace {
  struct XmPatternCell {
    inline bool hasNote() const { return config & 0x01; }
    inline bool hasInstrument() const { return config & 0x02; }
    inline bool hasVolume() const { return config & 0x04; }
    inline bool hasEffectType() const { return config & 0x08; }
    inline bool hasEffectParameter() const { return config & 0x10; }

    unsigned char config;
    unsigned char note;
    unsigned char instrument;
    unsigned char volume = 64;
    unsigned char effectType;
    unsigned char effectParameter;
  };

  struct XmPattern {
    ~XmPattern() {
      delete[] cells;
    }
    uint16_t numRows;
    uint16_t numChannel;
    XmPatternCell *cells = nullptr;
    inline const XmPatternCell &cell(int channel, int row) const { return cells[channel + row * numChannel]; }
  };

  struct XmSample {
    QString name;
    uint32_t index;
    uint32_t length;
    uint32_t loopStart;
    uint32_t loopLength;
    uint8_t volume;
    uint8_t finetune;
    uint8_t flags;
    uint8_t panning;
    int8_t relativeNoteNumber;
  };

  struct XmInstrument {
    ~XmInstrument() {
      delete[] samples;
    }

    QString name;
    uint8_t sampleIndex[96];
    
    uint16_t numSamples = 0;
    XmSample *samples = nullptr;
  };

  struct XmFile {
    ~XmFile() {
      delete[] patterns;
      delete[] instruments;
    }

    XmPattern *patterns = nullptr;
    XmInstrument *instruments = nullptr;
    QString moduleName;
    char patternOrderTable[256];
    uint16_t numChannels;
    uint16_t numPatterns;
    uint16_t songLength;
    uint16_t restartPosition;
    uint16_t numInstruments;
    uint16_t tempo;
    uint16_t bpm;
  };

// -----------------------------------------------------------------------------
bool loadHeader(XmFile &xmFile, DataStream &data) {
  if (data.readString(17) != "Extended Module: ") {
    qWarning() << "XM file header does not match";
    return false;
  }

  xmFile.moduleName = data.readTrimmedString(20);
  if (data.read<uint8_t>() != 0x1A) {
    qWarning() << "XM file header does not match";
    return false;
  }

  data.readTrimmedString(20); // tracker name
  data.read<uint16_t>(); // version

  uint32_t headerSize = data.read<uint32_t>();
  xmFile.songLength = data.read<uint16_t>();
  if (xmFile.songLength >= 256) {
    qWarning() << "Song length must be below 256";
    return false;
  }
  
  xmFile.restartPosition = data.read<uint16_t>();
  xmFile.numChannels = data.read<uint16_t>();
  if (xmFile.numChannels > Sft::MAX_NUM_CHANNEL) {
    qWarning() << "Song contains" << xmFile.numChannels << "but only" << Sft::MAX_NUM_CHANNEL << "are supported.";
  }

  xmFile.numPatterns = data.read<uint16_t>();
  if (xmFile.numPatterns >= 256) {
    qWarning() << "XmPattern count must be below 256";
    return false;
  }
  xmFile.numInstruments = data.read<uint16_t>();
  data.read<uint16_t>(); // flags
  xmFile.tempo = data.read<uint16_t>();
  xmFile.bpm = data.read<uint16_t>();
  qDebug() << xmFile.tempo << xmFile.bpm;
  data.readRawData(xmFile.patternOrderTable, 256);
  data.skipRawData(headerSize - 256 - 20);

  return true;
}

// -----------------------------------------------------------------------------
bool loadInstruments(XmFile &xmFile, DataStream &data, Song *song) {
  xmFile.instruments = new XmInstrument[xmFile.numInstruments];
  uint16_t sampleCounter = 0;

  for (int index=0; index<xmFile.numInstruments; index++) {
    auto &xmInstrument = xmFile.instruments[index];

    uint32_t instrumentHeaderSize = data.read<uint32_t>();
    QString instrumentName = data.readTrimmedString(22);
    data.read<uint8_t>();
    xmInstrument.numSamples = data.read<uint16_t>();

    if (xmInstrument.numSamples == 0) {
      data.skipRawData(instrumentHeaderSize - 29);
      continue;
    }

    xmInstrument.samples = new XmSample[xmInstrument.numSamples];

    uint32_t sampleHeaderSize = data.read<uint32_t>();
    data.readRawData((char*)xmInstrument.sampleIndex, 96);

    data.skipRawData(instrumentHeaderSize - 33 - 96);
    for (int sampleIndex=0; sampleIndex<xmInstrument.numSamples; sampleIndex++) {
      auto &xmSample = xmInstrument.samples[sampleIndex];

      xmSample.index = ++sampleCounter;
      xmSample.length = data.read<uint32_t>();
      xmSample.loopStart = data.read<uint32_t>();
      xmSample.loopLength = data.read<uint32_t>();
      xmSample.volume = data.read<uint8_t>();
      xmSample.finetune = data.read<uint8_t>();
      xmSample.flags = data.read<uint8_t>();
      xmSample.panning = data.read<uint8_t>();
      xmSample.relativeNoteNumber = data.read<uint8_t>();
      data.read<uint8_t>();
      data.readString(22);
      xmSample.name = data.skipRawData(sampleHeaderSize - 40);
    }

    for (int sampleIndex=0; sampleIndex<xmInstrument.numSamples; sampleIndex++) {
      auto &xmSample = xmInstrument.samples[sampleIndex];
      if (xmSample.length == 0) {
        continue;
      }

      bool is16Bit = xmSample.flags & 0x10;

      Sft::Sample *sample = song->samples().create("sample" + QString::number(xmSample.index));
      double note = 48 - 1 + xmSample.relativeNoteNumber - 48 + (xmSample.finetune / 128.0);
      double speed = qPow(2, note / 12.0) * 8363;

      auto *sampleData = sample->inputData();
      if (is16Bit) {
        sampleData->allocate(1, speed, xmSample.length / 2);
        int16_t old = 0;
        for (uint32_t i=0; i<xmSample.length / 2; i++) {
          int16_t value = data.read<int16_t>() + old;
          sampleData->sample(0, i) = value / 32768.0;
          old = value;
        }
      } else {
        sampleData->allocate(1, speed, xmSample.length);
        int8_t old = 0;
        for (uint32_t i=0; i<xmSample.length; i++) {
          int8_t value = data.read<int8_t>() + old;
          sampleData->sample(0, i) = value / 128.0;
          old = value;
        }
      }

      Sft::InstrumentPtr instrument = song->instruments().create(sample);
      instrument->setName(instrumentName.isEmpty() ? "Instrument" + QString::number(xmSample.index) : instrumentName);
      instrument->setNoteAdjust(24);
      
      if (xmSample.loopLength) {
        instrument->setLooped(true);
        sample->setLooped(true);
        if (!is16Bit) {
          sample->setLoopStart(xmSample.loopStart / speed);
        } else {
          sample->setLoopStart(xmSample.loopStart / 2 / speed);
        }
        instrument->setVolume(xmSample.volume / 64.0);
      }
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool loadPatterns(XmFile &xmFile, DataStream &data) {
  xmFile.patterns = new XmPattern[xmFile.numPatterns];
  for (uint16_t patternIndex=0; patternIndex<xmFile.numPatterns; patternIndex++) {
    XmPattern &pattern = xmFile.patterns[patternIndex];

    data.read<uint32_t>();
    data.read<uint8_t>();
    pattern.numRows = data.read<uint16_t>();
    pattern.numChannel = xmFile.numChannels;
    pattern.cells = new XmPatternCell[pattern.numRows * xmFile.numChannels];
    data.read<uint16_t>();

    uint8_t note=0;
    uint8_t instrument=0;
    uint8_t volume=0;
    uint8_t effectType=0;
    uint8_t effectParameter=0;
    uint32_t index=0;
    for (uint16_t channel=0; channel<xmFile.numChannels; channel++) {
      for (uint16_t row=0; row<pattern.numRows; row++) {
        uint8_t config = data.read<uint8_t>();
        XmPatternCell &cell = pattern.cells[index++];

        if (config < 128) {
          note = config;
          instrument = data.read<uint8_t>();
          volume = data.read<uint8_t>();
          effectType = data.read<uint8_t>();
          effectParameter = data.read<uint8_t>();
          config = 0xFF;
        } else {
          if (config & 0x01) { note = data.read<uint8_t>(); }
          if (config & 0x02) { instrument = data.read<uint8_t>(); }
          if (config & 0x04) { volume = data.read<uint8_t>(); }
          if (config & 0x08) { effectType = data.read<uint8_t>(); }
          if (config & 0x10) { effectParameter = data.read<uint8_t>(); }
        }

        cell.config = config;
        cell.note = note;
        cell.instrument = instrument;
        cell.volume = volume;
        cell.effectType = effectType;
        cell.effectParameter = effectParameter;
      }
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
QString noteText(int8_t note) {
  if (note == 97) {
    return "r";
  }

  note = (note - 1) % 12;
  switch (note) {
    case 0: return "c";
    case 1: return "c+";
    case 2: return "d";
    case 3: return "d+";
    case 4: return "e";
    case 5: return "f";
    case 6: return "f+";
    case 7: return "g";
    case 8: return "g+";
    case 9: return "a";
    case 10: return "a+";
    case 11: return "b";
  }

  return "a";
}

// -----------------------------------------------------------------------------
int noteOctave(int8_t note) {
  if (note == 97) {
    return -1;
  }

  return ((note - 1) / 12) + 1;
}

// -----------------------------------------------------------------------------
QString noteDuration(const XmFile &xmFile, int duration) {
  duration *= xmFile.tempo;

  switch (duration - 1) {
    case 0xbf: return "1";
    case 0x8f: return "2";
    case 0x5f: return "3";
    case 0x47: return "4";
    case 0x2f: return "5";
    case 0x23: return "6";
    case 0x1f: return "7";
    case 0x17: return "8";
    case 0x0f: return "9";
    case 0x0b: return "10";
    case 0x07: return "11";
    case 0x05: return "12";
    case 0x02: return "13";
  }

  return ":" + QString::number(duration);
}

// -----------------------------------------------------------------------------
QString compileChannel(const XmFile &xmFile, int channelIndex) {
  QString current;
  QString rowText;
  QString result;
  
  if (channelIndex == 0) {
    //qDebug() << "BPM" << xmFile.bpm << "TEMPO" << xmFile.tempo;
    current += "t" + QString::number(xmFile.bpm) + " ";
  }

  int activeInstrument = -1;
  QString lastInstrument;
  int16_t lastVolume = -1;
  int16_t lastOctave = -1;
  
  int16_t currentNote = 97;
  int32_t currentNoteTick = 0;
  int32_t tick = 0;

  for (int tableIndex=0; tableIndex<xmFile.songLength; tableIndex++) {
    int patternIndex = xmFile.patternOrderTable[tableIndex];
    if (patternIndex >= xmFile.numPatterns) {
      continue;
    }

    const XmPattern &pattern = xmFile.patterns[patternIndex];
    for (int row=0; row<pattern.numRows; row++) {
      const XmPatternCell &cell = pattern.cell(channelIndex, row);

      if (cell.hasInstrument() && (cell.instrument - 1) < xmFile.numInstruments) {
        activeInstrument = (cell.instrument - 1);
      }

      if (cell.hasNote()) {
        if (tick > currentNoteTick) {
          rowText += noteText(currentNote) + noteDuration(xmFile, tick - currentNoteTick) + " ";
        }

        if (rowText.length() > 60) {
          result += rowText + "\n";
          rowText = "";
        }

        currentNote = cell.note;
        currentNoteTick = tick;
        rowText += current;
        current = "";

        int octave = noteOctave(cell.note);
        if (octave >= 0 && lastOctave != octave) {
          if (octave == lastOctave + 1) {
            rowText += "> ";
          } else if (octave == lastOctave - 1) {
            rowText += "< ";
          } else {
            rowText += "o" + QString::number(octave) + " ";
          }
          lastOctave = octave;
        }

        if (activeInstrument >= 0) {
          const XmInstrument &instrument = xmFile.instruments[activeInstrument];
          if (currentNote > 0 && currentNote <= 96) {
            int sampleIndex = instrument.sampleIndex[currentNote - 1];
            if (sampleIndex < instrument.numSamples) {
              QString instrumentName = "sample" + QString::number(instrument.samples[sampleIndex].index);
              if (instrumentName != lastInstrument) {
                rowText += "@\"" + instrumentName + "\"\n";
                lastInstrument = instrumentName;
              }
            }
          }
        }

        int volume = cell.hasVolume() ? cell.volume : 0x50;
        if (volume != lastVolume && currentNote != 97) {
          lastVolume = volume;
          rowText += "v" + QString::number((int)((lastVolume - 0x10) / (double)0x40 * 64)) + " ";
        }
      }

      tick++;
    }
  }

  if (result.isEmpty() && rowText.isEmpty() && current.isEmpty()) {
    if (currentNote == -1 || currentNote == 97) {
      return "";
    }
  }

  if (currentNote != -1) {
    rowText += noteText(currentNote) + noteDuration(xmFile, tick - currentNoteTick) + " ";
  }

  return result + rowText + current;
}
}

// -----------------------------------------------------------------------------
bool XmSongLoader::load(const QString &filename) const {
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    return false;
  }

  XmFile xmFile;

  DataStream data(&file);
  data.setByteOrder(QDataStream::LittleEndian);

  //! Loads the data
  if (!loadHeader(xmFile, data)
    || !loadPatterns(xmFile, data)
    || !loadInstruments(xmFile, data, mSong)
  ) {
    return false;
  }


  // Generates the result
  int target = 0;
  for (int origin=0; origin<xmFile.numChannels && target < Sft::MAX_NUM_CHANNEL; origin++) {
    QString code = compileChannel(xmFile, origin);
    if (code.isEmpty()) {
      continue;
    }

    mSong->channel(target)->script()->set(code);
    mSong->channel(target)->script()->requireCompiled();

    target++;
  }

  mSong->samples().autoAdjustSampleQuality();

  return true;
}

