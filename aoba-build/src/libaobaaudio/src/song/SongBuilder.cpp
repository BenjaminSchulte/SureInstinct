#include <QDebug>
#include <aoba/log/Log.hpp>
#include <aoba/song/SongBuilder.hpp>
#include <fma/output/DynamicBuffer.hpp>
#include <sft/mml/MmlParser.hpp>

using namespace Aoba;

// ---------------------------------------------------------------------------
SongBuilderChannel::SongBuilderChannel()
  : mBuffer(new FMA::output::DynamicBuffer)
{
}

// ---------------------------------------------------------------------------
SongBuilderChannel::~SongBuilderChannel() {
  delete mBuffer;
}

// ---------------------------------------------------------------------------
void *SongBuilderChannel::getData() const {
  return mBuffer->getData();
}

// ---------------------------------------------------------------------------
int SongBuilderChannel::getSize() const {
  return mBuffer->getSize();
}

// ---------------------------------------------------------------------------
void SongBuilderChannel::write(unsigned char byte) {
  mBuffer->write(&byte, 1);
}

// ---------------------------------------------------------------------------
void SongBuilderChannel::override(unsigned char byte, int offset) {
  int old = mBuffer->tell();
  mBuffer->seek(offset);
  mBuffer->write(&byte, 1);
  mBuffer->seek(old);
}

// ---------------------------------------------------------------------------
int SongBuilderChannel::reserve() {
  int offset = mBuffer->tell();
  unsigned char c = 0xFF;
  mBuffer->write(&c, 1);
  return offset;
}

// ---------------------------------------------------------------------------
void SongBuilder::error(int row, int col, const QString &message) {
  Aoba::log::warn(QString::number(row) + "," + QString::number(col) +": " + message);
  mFailed = true;
}

// ---------------------------------------------------------------------------
void SongBuilder::setAttribute(const QString &id, const QString &value) {
  (void)id;
  (void)value;
}

// ---------------------------------------------------------------------------
void SongBuilder::beginTrack(int index) {
  if (index < 0 || index >= 8) {
    Aoba::log::warn("Track is out of range " + QString::number(index));
    mFailed = true;
    return;
  }

  mCurrentChannel = index;
}

// ---------------------------------------------------------------------------
void SongBuilder::writeNoteValue(int noteValue, int duration) {
  int durationValue = Sft::MmlParser::durationToMmlDuration(duration) + 1;

  mChannels[mCurrentChannel].write(noteValue + durationValue * 0x0E);

  if (durationValue == 0x00) {
    mChannels[mCurrentChannel].write(duration - 1);
  }
}

// ---------------------------------------------------------------------------
void SongBuilder::note(QChar note, bool half, int duration) {
  int noteValue = -1;

  switch (note.unicode()) {
    case 'c': noteValue = (!half) ? 0 : 1; break;
    case 'd': noteValue = (!half) ? 2 : 3; break;
    case 'e': noteValue = (!half) ? 4 : 4; break;
    case 'f': noteValue = (!half) ? 5 : 6; break;
    case 'g': noteValue = (!half) ? 7 : 8; break;
    case 'a': noteValue = (!half) ? 9 : 10; break;
    case 'b': noteValue = (!half) ? 11 : 11; break;
    case 'h': noteValue = (!half) ? 11 : 11; break;
  }

  if (noteValue == -1) {
    Aoba::log::error("Unsupported note: " + QString(note));
    mFailed = true;
    return;
  }

  writeNoteValue(noteValue, duration);
}

// ---------------------------------------------------------------------------
void SongBuilder::setNoiseEnabled(bool enabled, int power) {
  if (enabled) {
    mChannels[mCurrentChannel].write(0xD8);
    mChannels[mCurrentChannel].write(power);
  } else {
    Aoba::log::warn("disable setNoiseEnabled not implemented yet");
  }
}

// ---------------------------------------------------------------------------
void SongBuilder::setPitchChannelEnabled(bool enabled) {
  if (enabled) {
    mChannels[mCurrentChannel].write(0xD9);
  } else {
    Aoba::log::warn("disable setPitchChannelEnabled not implemented yet");
  }
}

// ---------------------------------------------------------------------------
void SongBuilder::decrementOctave() {
  mChannels[mCurrentChannel].write(0xC7);
}

// ---------------------------------------------------------------------------
void SongBuilder::incrementOctave() {
  mChannels[mCurrentChannel].write(0xC8);
}

// ---------------------------------------------------------------------------
void SongBuilder::setTrackRepeatOffset() {
  mChannels[mCurrentChannel].write(0xCD);
}

// ---------------------------------------------------------------------------
void SongBuilder::startLoop() {
  mChannels[mCurrentChannel].write(0xCF);
  mLoopStack.push_back(
    mChannels[mCurrentChannel].reserve()
  );
}

// ---------------------------------------------------------------------------
void SongBuilder::endLoop(int repeatTimes) {
  if (!mLoopStack.length()) {
    Aoba::log::warn("End Loop without matching Start Loop");
    mFailed = true;
    return;
  }
  
  int offset = mLoopStack.back();
  mLoopStack.pop_back();
  mChannels[mCurrentChannel].write(0xD0);
  mChannels[mCurrentChannel].override(repeatTimes - 1, offset);
}

// ---------------------------------------------------------------------------
void SongBuilder::resumeNote(int duration) {
  writeNoteValue(0x0D, duration);
}

// ---------------------------------------------------------------------------
void SongBuilder::pause(int duration) {
  writeNoteValue(0x0C, duration);
}

// ---------------------------------------------------------------------------
void SongBuilder::setInstrument(const QString &instrument) {
  int id = mAdapter->resolveInstrumentId(instrument);
  if (id == -1) {
    Aoba::log::warn("Could not find instrument with ID: " + instrument);
    mFailed = true;
    return;
  }

  if (!mInstrumentList.contains(id)) {
    mInstrumentList.push_back(id);
  }

  mChannels[mCurrentChannel].write(0xC4);
  mChannels[mCurrentChannel].write(id);
}

// ---------------------------------------------------------------------------
void SongBuilder::setOctave(int octave) {
  mChannels[mCurrentChannel].write(0xC9);
  mChannels[mCurrentChannel].write(octave - 1);
}

// ---------------------------------------------------------------------------
void SongBuilder::setTempo(int tempo) {
  mChannels[mCurrentChannel].write(0xC5);
  mChannels[mCurrentChannel].write(tempo);
}

// ---------------------------------------------------------------------------
void SongBuilder::setVolume(int volume) {
  mChannels[mCurrentChannel].write(0xC6);
  mChannels[mCurrentChannel].write(volume);
}

// ---------------------------------------------------------------------------
void SongBuilder::setPanning(int panning) {
  mChannels[mCurrentChannel].write(0xCE);
  mChannels[mCurrentChannel].write(128+panning);
}

// ---------------------------------------------------------------------------
void SongBuilder::setEchoEnabled(bool enabled) {
  if (enabled) {
    mChannels[mCurrentChannel].write(0xD2);
  } else {
    mChannels[mCurrentChannel].write(0xDA);
  }
}

// ---------------------------------------------------------------------------
void SongBuilder::setAdsrA(int a) {
  mChannels[mCurrentChannel].write(0xD1);
  mChannels[mCurrentChannel].write(a);
}

// ---------------------------------------------------------------------------
void SongBuilder::setAdsrD(int d) {
  mChannels[mCurrentChannel].write(0xD5);
  mChannels[mCurrentChannel].write(d << 4);
}

// ---------------------------------------------------------------------------
void SongBuilder::setAdsrS(int s) {
  mChannels[mCurrentChannel].write(0xD6);
  mChannels[mCurrentChannel].write(s << 5);
}

// ---------------------------------------------------------------------------
void SongBuilder::setAdsrR(int r) {
  mChannels[mCurrentChannel].write(0xCC);
  mChannels[mCurrentChannel].write(r);
}
  
// ---------------------------------------------------------------------------
void SongBuilder::setPitchOffset(float notes, bool relative) {
  uint8_t notesByte;
  bool large = false;

  if (notes < -32 || notes > 31) {
    Aoba::log::error("setPitchOffset out of range");
    mFailed = true;
    return;
  } else if (notes < -8 || notes > 7) {
    large = true;
    notesByte = notes * 0x04;
  } else {
    notesByte = notes * 0x10;
  }

  if (relative) {
    Aoba::log::error("TODO: setPitchOffset relative not yet supported");
    mFailed = true;
  } else {
    mChannels[mCurrentChannel].write(large ? 0xD5 : 0xCB);
  }
  mChannels[mCurrentChannel].write(notesByte);
  
}

// ---------------------------------------------------------------------------
void SongBuilder::easePitch(uint8_t duration, int notes) {
  mChannels[mCurrentChannel].write(0xD3);
  mChannels[mCurrentChannel].write(duration);
  mChannels[mCurrentChannel].write(notes);
}

// ---------------------------------------------------------------------------
void SongBuilder::vibratePitch(uint8_t ticks, float notes) {
  if ((notes / ticks) < -2 || (notes / ticks) > 1) {
    Aoba::log::warn("Vibrato notes out of range");
    mFailed = true;
    return;
  }

  mChannels[mCurrentChannel].write(0xD4);
  mChannels[mCurrentChannel].write(ticks);

  uint8_t notesByte = (notes / ticks) * 0x40;
  mChannels[mCurrentChannel].write(notesByte);
}

// ---------------------------------------------------------------------------
void SongBuilder::writeChannelTo(int i, FMA::output::DynamicBuffer *block) const {
  unsigned char c = 0xCA;
  block->write(mChannels[i].getData(), mChannels[i].getSize());
  block->write(&c, 1);
}

// ---------------------------------------------------------------------------
void SongBuilder::writeSongDataTo(FMA::output::DynamicBuffer *block) const {
  FMA::output::DynamicBuffer header;

  // Prepares the header
  unsigned char c = 0xFF;
  header.write(&c, 1);

  // Writes the channel index
  uint16_t channelOffset = 0x2000 + header.getSize() + 8 * 2;
  for (int i=0; i<8; i++) {
    block->write(&channelOffset, 2);
    channelOffset += mChannels[i].getSize() + 1;
  }

  // Writes the header
  block->write(header.getData(), header.getSize());
  c = 0xCA; // stop or loop command
  for (int i=0; i<8; i++) {
    writeChannelTo(i, block);
  }
}

// ---------------------------------------------------------------------------
void SongBuilder::writeInstrumentList(FMA::output::DynamicBuffer *block, const QVector<int> &instrumentList) {
  block->writeNumber(instrumentList.length(), 1);
  for (int id : instrumentList) {
    block->writeNumber(id, 1);
  }
}

// ---------------------------------------------------------------------------
void SongBuilder::writeTo(FMA::output::DynamicBuffer *block) const {
  FMA::output::DynamicBuffer content;

  writeSongDataTo(&content);

  // Write final data
  writeInstrumentList(block, mInstrumentList);

  uint16_t size = content.getSize();
  uint16_t sizeDiv3 = (size + 2) / 3;
  block->writeNumber(sizeDiv3, 2);
  block->write(content.getData(), size);
}
