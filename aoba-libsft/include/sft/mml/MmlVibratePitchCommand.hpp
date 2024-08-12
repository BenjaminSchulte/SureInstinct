#pragma once

#include "MmlCommand.hpp"

namespace Sft {

class MmlVibratePitchCommand : public MmlCommand {
public:
  //! Constructor
  MmlVibratePitchCommand(const MmlParserInfo &info, uint8_t duration, float notes) : MmlCommand(info), mDuration(duration), mNotes(notes) {}

  //! Returns the type
  MmlCommandType type() const override { return MmlCommandType::VIBRATE_PITCH; }

  //! Returns the tempo
  inline uint8_t vibrateDuration() const { return mDuration; }

  //! Returns the tempo
  inline float vibrateNotes() const { return mNotes; }

  //! Updates the execute state
  void updateExecuteState(Song *song, MmlExecutedState &state) const override;

private:
  //! The tempo size
  uint8_t mDuration;

  //! The tempo size
  float mNotes;
};

}