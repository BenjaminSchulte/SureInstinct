#include <QDebug>
#include <QTextCodec>
#include <QDataStream>
#include <QByteArray>
#include <QFile>
#include <sft/song/Song.hpp>
#include <sft/mml/MmlParser.hpp>
#include <sft/mml/MmlParserInterface.hpp>
#include <sft/Configuration.hpp>

using namespace Sft;

namespace {
  const int delayLookupTable[] = {
    0xbf,
    0x8f,
    0x5f,
    0x47,
    0x2f,
    0x23,
    0x1f,
    0x17,
    0x0f,
    0x0b,
    0x07,
    0x05,
    0x02,
  };

  bool isIdentifierChar(const QChar &c) {
    if (c >= 'A' && c <= 'Z') return true;
    if (c >= 'a' && c <= 'z') return true;
    return false;
  }

  bool isWhiteSpace(const QChar &c) {
    if (c == ' ') return true;
    if (c == '\t') return true;
    return false;
  }

  bool isDigit(const QChar &c) {
    if (c >= '0' && c <= '9') return true;
    return false;
  }
}

// -----------------------------------------------------------------------------
int MmlParser::durationToMmlDuration(int duration) {
  duration--;

  for (int i=0; i<13; i++) {
    if (delayLookupTable[i] == duration) {
      return i;
    }
  }

  return -1;
}

// -----------------------------------------------------------------------------
int MmlParser::mmlDurationToDuration(int duration) {
  if (duration < 1 || duration > 13) {
    return -1;
  }

  return delayLookupTable[duration - 1] + 1;
}

// -----------------------------------------------------------------------------
void MmlParser::setInfo(const State &state, int left, int right) const {
  left += state.commandStart;
  right += state.index - 1;
  
  mAdapter->setInfo(MmlParserInfo(left, right, state.mml.mid(left, right - left + 1)));
}

// -----------------------------------------------------------------------------
void MmlParser::error(const State &state, const QString &message) const {
  setInfo(state);
  mAdapter->error(state.row, state.index - state.rowStart + 1, message);
}

// -----------------------------------------------------------------------------
bool MmlParser::optionalWhiteSpace(State &state) const {
  while (isWhiteSpace(state.get(state.index))) {
    state.index++;
  }
  return true;
}

// -----------------------------------------------------------------------------
bool MmlParser::expectWhiteSpace(State &state) const {
  QChar c(state.get(state.index++));
  if (!isWhiteSpace(c)) {
    return false;
  }

  return optionalWhiteSpace(state);
}

// -----------------------------------------------------------------------------
bool MmlParser::expectNewLine(State &state) const {
  optionalWhiteSpace(state);
  
  QChar c = state.get(state.index++);
  if (c == '\n') {
    state.row++;
    state.rowStart = state.index;
    return true;
  } else if (c == '#') {
    return parseToNewLine(state);
  }

  return false;
}

// -----------------------------------------------------------------------------
bool MmlParser::parseToNewLine(State &state) const {
  QChar c;
  do {
    c = state.get(state.index++);
  } while(c != '\n' && c != '\0');
  state.row++;
  state.rowStart = state.index;
  return true;
}

// -----------------------------------------------------------------------------
bool MmlParser::parseText(State &state, QString &text) const {
  text = "";

  if (state.get(state.index) != '"') {
    return false;
  }

  QChar c;
  while ((c = state.get(++state.index)) != '"') {
    if (c == '\0') {
      --state.index;
      break;
    }
    text += c;
  }

  state.index++;

  return true;
}

// -----------------------------------------------------------------------------
bool MmlParser::parseDuration(State &state, int &number) const {
  QChar c(state.get(state.index));

  if (c == ':') {
    state.index++;
    if (!parseUnsignedNumber(state, number)) {
      return false;
    }
    return true;
  }
  
  if (isDigit(c)) {
    if (!parseUnsignedNumber(state, number)) {
      return false;
    }
  } else {
    number = state.defaultDuration;
  }

  if (number >= 1 && number <= 13) {
    number = delayLookupTable[number - 1] + 1;
  } else {
    qWarning() << "MML duration out of index" << number;
    number = delayLookupTable[0] + 1;
  }

  return true;
}

// -----------------------------------------------------------------------------
bool MmlParser::parseUnsignedNumber(State &state, int &number, int min, int max) const {
  number = 0;

  bool hasDigit = false;
  QChar c;
  while (isDigit(c = state.get(state.index++))) {
    number = number * 10 + (c.unicode() - '0');
    hasDigit = true;
  }

  if (number < min || number > max) {
    error(state, QString("Number must be between %1 and %2").arg(min).arg(max));
  }

  state.index--;
  return hasDigit;
}

// -----------------------------------------------------------------------------
bool MmlParser::parseSignedNumber(State &state, int &number) const {
  bool minus = false;
  if (state.get(state.index) == '-') {
    minus = true;
    state.index++;
  }

  if (!parseUnsignedNumber(state, number)) {
    return false;
  }

  if (minus) {
    number = -number;
  }

  return true;
}

// -----------------------------------------------------------------------------
bool MmlParser::parseUnsignedFloat(State &state, double &number, double min, double max) const {
  int integer;
  
  if (!parseUnsignedNumber(state, integer)) {
    return false;
  }
  number = integer;

  if (state.get(state.index) == '.') {
    int decimal;
    state.index++;
    int oldAddress = state.index;
    if (!parseUnsignedNumber(state, decimal)) {
      return false;
    }

    int length = state.index - oldAddress;
    double dec = decimal;
    while (length-- > 0) {
      dec /= 10;
    }
    number += dec;
  }

  if (number < min || number > max) {
    error(state, QString("Number must be between %1 and %2").arg(min).arg(max));
  }

  return true;
}

// -----------------------------------------------------------------------------
bool MmlParser::parseSignedFloat(State &state, double &number, double min, double max) const {
  bool minus = false;
  if (state.get(state.index) == '-') {
    minus = true;
    state.index++;
  }

  if (!parseUnsignedFloat(state, number)) {
    return false;
  }

  if (minus) {
    number = -number;
  }

  if (number < min || number > max) {
    error(state, QString("Number must be between %1 and %2").arg(min).arg(max));
  }

  return true;
}

// -----------------------------------------------------------------------------
bool MmlParser::parseBoolean(State &state, int &number) const {
  QChar c(state.get(state.index++));

  if (c == '1' || c == 'e' || c == 't' || c == 'y') { number = 1; }
  else if (c == '0' || c == 'd' || c == 'f' || c == 'n') { number = 0; }
  else {
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
bool MmlParser::parseMetaData(State &state) const {
  int start = state.index + 1;
  QChar c;

  while (isIdentifierChar(c = state.get(++state.index))) {
    // resume
  }

  QString command(state.mml.mid(start, state.index - start).toUpper());

  if (command == "TITLE") {
    QString text;
    if (!expectWhiteSpace(state) || !parseText(state, text)) {
      return false;
    }
  
    setInfo(state);
    mAdapter->setAttribute("title", text);
  } else if (command == "TRACK") {
    int number;
    if (!expectWhiteSpace(state) || !parseUnsignedNumber(state, number)) {
      return false;
    }

    setInfo(state);
    mAdapter->beginTrack(number);
  } else {
    qWarning() << "Unsupported meta command" << command;
    return parseToNewLine(state);
  }

  return expectNewLine(state);
}

// -----------------------------------------------------------------------------
bool MmlParser::parseNote(State &state, QChar note) const {
  QChar next(state.get(state.index));
  bool half = (next == '+');
  if (half) {
    state.index++;
  }

  int duration;
  if (!parseDuration(state, duration)) {
    return false;
  }

  setInfo(state);
  mAdapter->note(note, half, duration);
  return true;
}

// -----------------------------------------------------------------------------
uint8_t MmlParser::convertBpmToTempo(unsigned int bpm) {
  // Internal counter counts up at 8000 Hz
  // One beat represents 192 counter ticks

  double secondsPerBeat = 1.0 / (bpm / 60.0);
  double result = secondsPerBeat / (MEASURE_DURATION / 4.0) / (1.0 / DSP_TIMER_HZ);
  int value = result + 0.5;

  return qMax(1, qMin(255, value));
}

// -----------------------------------------------------------------------------
double MmlParser::convertTempoToBpm(unsigned int tempo) {
  // Internal counter counts up at 8000 Hz
  // One beat represents 192 counter ticks

  double secondsPerBeat = (double)tempo * ((1.0 / (double)DSP_TIMER_HZ) * ((double)MEASURE_DURATION / 4.0));
  return 1.0 / secondsPerBeat * 60.0;
}

// -----------------------------------------------------------------------------
bool MmlParser::parseCommand(State &state) const {
  optionalWhiteSpace(state);

  int number;
  double floatNumber;
  QString text;

  state.begin();

  QChar c(state.get(state.index++));
  QChar xc('\0');

  if (c == 'x') {
    xc = state.get(state.index++);
  }
  if (c == 'v') {
    xc = state.get(state.index);
    if (xc == 'a' || xc == 'd' || xc == 's' || xc == 'r') {
      state.index++;
    }
  }
  if (c == 'p') {
    xc = state.get(state.index);
    if (xc == 'e' || xc == 'o' || xc == 'v') {
      state.index++;
    }
  }

  if (c == '\n') {
    state.row++;
    state.rowStart = state.index;
    return true;
  } else if (c == '#') {
    parseToNewLine(state);
    setInfo(state);
    mAdapter->comment();
    return true;
  } else if (c >= 'a' && c <= 'h') {
    return parseNote(state, c);
  } else if (c == '/') {
    setInfo(state);
    mAdapter->setTrackRepeatOffset();
  } else if (c == '<') {
    setInfo(state);
    mAdapter->decrementOctave();
  } else if (c == '>') {
    setInfo(state);
    mAdapter->incrementOctave();
  } else if (c == '@' && parseText(state, text)) {
    setInfo(state);
    mAdapter->setInstrument(text);
  } else if (c == 'r' && parseDuration(state, number)) {
    setInfo(state);
    mAdapter->pause(number);
  } else if (c == '^' && parseDuration(state, number)) {
    setInfo(state);
    mAdapter->resumeNote(number);
  } else if (c == 'l' && parseUnsignedNumber(state, number)) {
    state.defaultDuration = number;
  } else if (c == 'o' && parseUnsignedNumber(state, number, FIRST_OCTAVE, LAST_OCTAVE)) {
    setInfo(state);
    mAdapter->setOctave(number);
  } else if (c == 't' && parseUnsignedNumber(state, number, 39, 10000)) {
    setInfo(state);
    mAdapter->setTempo(convertBpmToTempo(number));
  } else if (c == 'y' && parseSignedNumber(state, number)) {
    setInfo(state);
    mAdapter->setPanning(number);
  } else if (c == 'x' && xc == 'e' && parseBoolean(state, number)) {
    setInfo(state);
    mAdapter->setEchoEnabled(!!number);
  } else if (c == 'x' && xc == 'p' && parseBoolean(state, number)) {
    setInfo(state);
    mAdapter->setPitchChannelEnabled(!!number);
  } else if (c == 'x' && xc == 'n' && parseBoolean(state, number)) {
    setInfo(state);
    xc = state.get(state.index);
    int power = -1;
    if (xc >= '0' || xc <= '9') {
      parseUnsignedNumber(state, power);
    }
    mAdapter->setNoiseEnabled(!!number, power);
  } else if (c == 'p' && xc == 'o') {
    bool relative = false;
    if (state.get(state.index) == 'r')  {
      relative = true;
      state.index++;
    }
    if (!parseSignedFloat(state, floatNumber, -32, 32)) {
      error(state, QString("Missing parameter for 'po'"));
      return false;
    }
    mAdapter->setPitchOffset(floatNumber, relative);
  } else if (c == 'p' && xc == 'e' && parseUnsignedNumber(state, number)) {
    uint8_t duration = number;
    xc = state.get(state.index++);
    if ((xc == '-' || xc == '+') && parseUnsignedNumber(state, number)) {
      if (xc == '-') {
        number = -number;
      }
      setInfo(state);
      mAdapter->easePitch(duration, number);
    } else {
      error(state, QString("Invalid parameter configuration for pitch ease'"));
      return false;
    }
  } else if (c == 'p' && xc == 'v' && parseUnsignedNumber(state, number)) {
    uint8_t duration = number;
    xc = state.get(state.index++);
    if ((xc == '-' || xc == '+') && parseUnsignedFloat(state, floatNumber, -32, 32)) {
      if (xc == '-') {
        floatNumber = -floatNumber;
      }
      setInfo(state);
      mAdapter->vibratePitch(duration, floatNumber);
    } else {
      error(state, QString("Invalid parameter configuration for pitch vibrato'"));
      return false;
    }

  } else if (c == 'v' && xc == 'a' && parseUnsignedNumber(state, number, 0, 0xF)) {
    setInfo(state);
    mAdapter->setAdsrA(number);
  } else if (c == 'v' && xc == 'd' && parseUnsignedNumber(state, number, 0, 7)) {
    setInfo(state);
    mAdapter->setAdsrD(number);
  } else if (c == 'v' && xc == 's' && parseUnsignedNumber(state, number, 0, 7)) {
    setInfo(state);
    mAdapter->setAdsrS(number);
  } else if (c == 'v' && xc == 'r' && parseUnsignedNumber(state, number, 0, 0x1F)) {
    setInfo(state);
    mAdapter->setAdsrR(number);
  } else if (c == 'v'  && parseUnsignedNumber(state, number, 0, 127)) {
    setInfo(state);
    mAdapter->setVolume(number);
  } else if (c == '[') {
    setInfo(state);
    mAdapter->startLoop();
  } else if (c == ']') {
    int times = 1;
    if (isDigit(state.get(state.index)))  {
      parseUnsignedNumber(state, times);
    }
    setInfo(state);
    mAdapter->endLoop(times);
  } else {
    error(state, QString("Unsupported command '") + c + "'");
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
bool MmlParser::parseRootLine(State &state) const {
  if (state.index >= state.mmlLength) {
    return false;
  }

  QChar c(state.get(state.index));
  if (c == '.') {
    return parseMetaData(state);
  } else {
    // Resume even on error
    parseCommand(state);
    return true;
  }
}

// -----------------------------------------------------------------------------
bool MmlParser::loadFromString(const QString &mml) const {
  State state(mml);

  while (parseRootLine(state)) {
    // loop
  }

  return true;
}

// -----------------------------------------------------------------------------
bool MmlParser::load(const QString &fileName) const {
  QFile file(fileName);
  if (!file.open(QIODevice::ReadOnly)) {
    mAdapter->error(0, 0, QString("Could not open file %1").arg(fileName));
    return false;
  }

  QString mml = QTextCodec::codecForMib(106)->toUnicode(file.readAll());
  return loadFromString(mml);
}
