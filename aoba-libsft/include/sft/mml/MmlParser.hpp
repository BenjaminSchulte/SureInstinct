#pragma once

namespace Sft {
class MmlParserInterface;

class MmlParser {
public:
  //! Constructor
  MmlParser(MmlParserInterface *adapter) : mAdapter(adapter) {}

  //! Loads the Song
  bool load(const QString &filename) const;

  //! Loads the song from a string
  bool loadFromString(const QString &mml) const;

  //! Duration number to MML duration value
  static int durationToMmlDuration(int duration);

  //! Duration number to MML duration value
  static int mmlDurationToDuration(int duration);

  //! Converts beats per minute to the audio engines tempo
  static uint8_t convertBpmToTempo(unsigned int bpm);

  //! Converts beats per minute to the audio engines tempo
  static double convertTempoToBpm(unsigned int tempo);

private:
  //! Parsing state
  struct State {
    State(const QString &mml) : mml(mml), mmlLength(mml.length()) {}

    inline void begin() { commandStart = index; }

    QString mml;
    int commandStart = 0;
    int mmlLength;
    int index = 0;
    int state = 0;
    int row = 1;
    int rowStart = 0;
    int defaultDuration = 5;
    bool isNewLine = true;
    QString text;

    inline QChar get(int index) const { return index >= mmlLength ? '\0' : mml[index]; }
  };

  //! The interface
  MmlParserInterface *mAdapter;

  //! Displays an error
  void error(const State &state, const QString &message) const;

  //! Parses a root line part
  bool optionalWhiteSpace(State &state) const;

  //! Parses a root line part
  bool expectWhiteSpace(State &state) const;

  //! Parses a root line part
  bool expectNewLine(State &state) const;

  //! Parses a root line part
  bool parseToNewLine(State &state) const;

  //! Parses a root line part
  bool parseCommand(State &state) const;

  //! Parses a root line part
  bool parseRootLine(State &state) const;

  //! Parses a metadata part
  bool parseMetaData(State &state) const;

  //! Parses a metadata part
  bool parseNote(State &state, QChar note) const;

  //! Parses a "Text" part
  bool parseText(State &state, QString &text) const;

  //! Parses a "Text" part
  bool parseBoolean(State &state, int &number) const;

  //! Parses a "Text" part
  bool parseDuration(State &state, int &number) const;

  //! Parses a "Text" part
  bool parseUnsignedNumber(State &state, int &number, int min=0, int max=0xFFFF) const;

  //! Parses a "Text" part
  bool parseSignedNumber(State &state, int &number) const;

  //! Parses a "Text" part
  bool parseUnsignedFloat(State &state, double &number, double min=0, double max=100000) const;

  //! Parses a "Text" part
  bool parseSignedFloat(State &state, double &number, double min=-100000, double max=100000) const;

  //! Sets the current info
  void setInfo(const State &state, int left=0, int right=0) const;
};

}