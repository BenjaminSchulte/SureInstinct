#pragma once

#include <QVector>
#include "MmlCommand.hpp"
#include "../instrument/Instrument.hpp"
#include "../Configuration.hpp"

namespace Sft {

struct MmlExecutedBasicState {
  unsigned int tick = 0;
  unsigned int volume = INITIAL_VOLUME;
  uint8_t octave = DEFAULT_OCTAVE - FIRST_OCTAVE;
  int16_t tempo = -1;
  unsigned int tempoUpdateTick = 0;
  InstrumentPtr instrument;
  MmlPitchInfo pitch;
  MmlVibrateInfo vibrate;
  bool noiseEnabled = false;
  uint8_t noiseClock = 0;
  bool pitchTrackEnabled = false;
  int8_t adsrA = 0xF;
  int8_t adsrD = 0x7;
  int8_t adsrS = 0x7;
  int8_t adsrR = 0;
  float pitchOffset = 0;
};

struct MmlExecutedState : public MmlExecutedBasicState {
  unsigned int commandIndex = 0;

  QVector<unsigned int> commandIndexStack;
  QVector<unsigned int> repeatTimeStack;
  QVector<uint8_t> oldOctaveStack;
};

struct MmlExecutedCommand {
  MmlExecutedCommand() {}

  //! Constructor
  MmlExecutedCommand(const MmlCommandPtr &command, const MmlExecutedBasicState &state)
    : mCommand(command), mState(state) {}

  //! Returns the command
  inline const MmlCommandPtr &command() const { return mCommand; }

  //! Returns the state
  inline const MmlExecutedBasicState &state() const { return mState; }

private:
  //! The command
  MmlCommandPtr mCommand;

  //! The state before executing the command
  MmlExecutedBasicState mState;
};

class MmlExecutedScript {
public:
  //! Constructor
  MmlExecutedScript() = default;

  //! Deconstructor
  virtual ~MmlExecutedScript() = default;

  //! Returns the command closest to a tick
  int commandIndexAtTick(unsigned tick) const;

  //! Returns a command
  int commandIndexForCodeOffset(int index) const;

  //! Returns the number of commands
  inline unsigned numCommands() const { return mCommands.size(); }

  //! Returns a command
  inline const MmlExecutedCommand &command(unsigned index) const { return mCommands[index]; }

  //! Returns the duration
  inline unsigned duration() const { return mDuration; }

  //! Returns the highest note
  inline uint8_t highestNote() const { return mHighestNote; }

  //! Returns the lowest note
  inline uint8_t lowestNote() const { return mLowestNote; }

protected:
  //! Lists of all commands
  QVector<MmlExecutedCommand> mCommands;

  //! List of all missing instruments
  QStringList mMissingInstruments;

  //! The duration
  unsigned mDuration = 0;

  //! The top most note
  uint8_t mHighestNote = 0;

  //! The lowest note
  uint8_t mLowestNote = 0xFF;
};

}