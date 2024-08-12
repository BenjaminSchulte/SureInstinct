#pragma once

#include "MmlCommand.hpp"

namespace Sft {

class MmlSetPitchOffsetCommand : public MmlCommand {
public:
  //! Constructor
  MmlSetPitchOffsetCommand(const MmlParserInfo &info, float notes, bool relative) : MmlCommand(info), mNotes(notes), mRelative(relative) {}

  //! Returns the type
  MmlCommandType type() const override { return MmlCommandType::SET_PITCH_OFFSET; }

  //! Returns the tempo
  inline float notes() const { return mNotes; }

  //! Returns the relative
  inline bool isRelative() const { return mRelative; }

  //! Updates the execute state
  void updateExecuteState(Song *song, MmlExecutedState &state) const override;

private:
  //! The tempo size
  float mNotes;

  //! Is relative?
  bool mRelative;
};

}