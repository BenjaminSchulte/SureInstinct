#pragma once

#include "MmlCommand.hpp"

namespace Sft {

class MmlRepeatLoopCommand : public MmlCommand {
public:
  //! Constructor
  MmlRepeatLoopCommand(const MmlParserInfo &info, int times) : MmlCommand(info), mTimes(times) {}

  //! Returns the type
  MmlCommandType type() const override { return MmlCommandType::REPEAT_LOOP; }

  //! Updates the execute state
  void updateExecuteState(Song *song, MmlExecutedState &state) const override;

  //! Returns the times
  inline int times() const { return mTimes; }

private:
  //! The times size
  int mTimes;
};

}