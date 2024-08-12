#pragma once

#include "MmlCommand.hpp"

namespace Sft {

class MmlSetOctaveCommand : public MmlCommand {
public:
  //! Constructor
  MmlSetOctaveCommand(const MmlParserInfo &info, int target) : MmlCommand(info), mTarget(target) {}

  //! Returns the type
  MmlCommandType type() const override { return MmlCommandType::SET_OCTAVE; }

  //! Returns the target
  inline int target() const { return mTarget; }

  //! Updates the execute state
  void updateExecuteState(Song *song, MmlExecutedState &state) const override;

private:
  //! The target size
  int mTarget;
};

}