#pragma once

#include "MmlCommand.hpp"

namespace Sft {

class MmlPitchNoteCommand : public MmlCommand {
public:
  //! Constructor
  MmlPitchNoteCommand(const MmlParserInfo &info, uint8_t duration, int8_t notes) : MmlCommand(info), mDuration(duration), mNotes(notes) {}

  //! Returns the type
  MmlCommandType type() const override { return MmlCommandType::PITCH_NOTE; }

  //! Returns the tempo
  inline uint8_t pitchDuration() const { return mDuration; }

  //! Returns the tempo
  inline uint8_t pitchNotes() const { return mNotes; }

  //! Updates the execute state
  void updateExecuteState(Song *song, MmlExecutedState &state) const override;

private:
  //! The tempo size
  uint8_t mDuration;

  //! The tempo size
  int8_t mNotes;
};

}