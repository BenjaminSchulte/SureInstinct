#pragma once

#include "MmlCommand.hpp"

namespace Sft {

class MmlModifyOctaveCommand : public MmlCommand {
public:
  //! Constructor
  MmlModifyOctaveCommand(const MmlParserInfo &info, int step) : MmlCommand(info), mStep(step) {}

  //! Returns the type
  MmlCommandType type() const override { return MmlCommandType::MODIFY_OCTAVE; }

  //! Returns the step
  inline int step() const { return mStep; }

  //! Updates the execute state
  void updateExecuteState(Song *song, MmlExecutedState &state) const override;

private:
  //! The step size
  int mStep;
};

}