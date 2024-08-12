#pragma once

#include <QString>

namespace Sft {
struct MmlParserInfo {
  MmlParserInfo() {}
  MmlParserInfo(int start, int end, const QString &content) : start(start), end(end), content(content) {}
  inline int length() const { return end - start + 1; }
  inline QChar charAt(int index) const { return index >= content.length() ? '\0' : content[index]; }

  int start;
  int end;
  QString content;
};

class MmlParserInterface {
public:
  //! Constructor
  MmlParserInterface() = default;

  //! Deconstructor
  virtual ~MmlParserInterface() = default;

  //! Displays an error
  virtual void error(int row, int col, const QString &message) = 0;

  //! Sets the title
  virtual void setAttribute(const QString &id, const QString &value) = 0;

  //! Starts a track
  virtual void beginTrack(int index) = 0;

  //! Adds a note
  virtual void note(QChar note, bool half, int duration) = 0;

  //! Adds a command
  virtual void comment() {}

  //! Decrements the octave
  virtual void decrementOctave() = 0;

  //! Increments the octave
  virtual void incrementOctave() = 0;

  //! Starts a loop
  virtual void setTrackRepeatOffset() = 0;

  //! Starts a loop
  virtual void startLoop() = 0;

  //! Ends a loop
  virtual void endLoop(int repeatTimes) = 0;

  //! Resumes the note
  virtual void resumeNote(int duration) = 0;

  //! Pauses
  virtual void pause(int duration) = 0;

  //! Sets the instrument
  virtual void setInstrument(const QString &instrument) = 0;

  //! Sets the octave
  virtual void setOctave(int octave) = 0;

  //! Sets the tempo
  virtual void setTempo(int tempo) = 0;

  //! Sets the volume
  virtual void setVolume(int volume) = 0;

  //! Sets the panning
  virtual void setPanning(int panning) = 0;

  //! Sets the echo enabled
  virtual void setEchoEnabled(bool enabled) = 0;

  //! Sets the pitch channel enabled
  virtual void setPitchChannelEnabled(bool enabled) = 0;

  //! Adds a ease pitch
  virtual void easePitch(uint8_t duration, int notes) = 0;

  //! Adds a vibrate pitch
  virtual void vibratePitch(uint8_t duration, float notes) = 0;

  //! Sets the pitch offset
  virtual void setPitchOffset(float notes, bool relative) = 0;

  //! Sets the ADSR value
  virtual void setAdsrA(int a) = 0;

  //! Sets the ADSR value
  virtual void setAdsrD(int d) = 0;

  //! Sets the ADSR value
  virtual void setAdsrS(int s) = 0;

  //! Sets the ADSR value
  virtual void setAdsrR(int r) = 0;

  //! Enables noise (negative to keep previous setting)
  virtual void setNoiseEnabled(bool enabled, int power=-1) = 0;

  //! Sets the info
  void setInfo(const MmlParserInfo &info) { mInfo = info; }

protected:
  //! Returns the current info
  inline const MmlParserInfo &info() const { return mInfo; }

private:
  //! The current info
  MmlParserInfo mInfo;
};

}