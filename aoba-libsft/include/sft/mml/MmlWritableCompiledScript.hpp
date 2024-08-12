#pragma once

#include "MmlCommand.hpp"
#include "MmlCompiledScript.hpp"
#include "MmlParserInterface.hpp"

namespace Sft {

class MmlWritableCompiledScript : public MmlCompiledScript, public MmlParserInterface {
public:
  //! Constructor
  MmlWritableCompiledScript(Song *song) : MmlCompiledScript(song) {}

  //! Displays an error
  void error(int row, int col, const QString &message) override;

  //! Sets the title
  void setAttribute(const QString &id, const QString &value) override;

  //! Starts a track
  void beginTrack(int index) override;

  //! Adds a note
  void note(QChar note, bool half, int duration) override;

  //! Decrements the octave
  void decrementOctave() override;

  //! Increments the octave
  void incrementOctave() override;

  //! Starts a loop
  void setTrackRepeatOffset() override;

  //! Starts a loop
  void startLoop() override;

  //! Ends a loop
  void endLoop(int repeatTimes) override;

  //! Resumes the note
  void resumeNote(int duration) override;

  //! Pauses
  void pause(int duration) override;

  //! Sets the instrument
  void setInstrument(const QString &instrument) override;

  //! Sets the octave
  void setOctave(int octave) override;

  //! Sets the tempo
  void setTempo(int tempo) override;

  //! Sets the volume
  void setVolume(int volume) override;

  //! Sets the panning
  void setPanning(int panning) override;

  //! Sets the echo enabled
  void setEchoEnabled(bool enabled) override;

  //! Sets the echo enabled
  void setPitchChannelEnabled(bool enabled) override;

  //! Sets the echo enabled
  void setNoiseEnabled(bool enabled, int power) override;

  //! Adds a ease pitch
  void easePitch(uint8_t duration, int notes) override;

  //! Adds a vibrate pitch
  void vibratePitch(uint8_t duration, float notes) override;

  //! Sets the pitch offset
  void setPitchOffset(float notes, bool relative) override;

  //! Sets the ADSR value
  void setAdsrA(int a) override;

  //! Sets the ADSR value
  void setAdsrD(int d) override;

  //! Sets the ADSR value
  void setAdsrS(int s) override;

  //! Sets the ADSR value
  void setAdsrR(int r) override;

private:
  inline void add(MmlCommand *command) { mCommands.push_back(MmlCommandPtr(command)); }
};

}