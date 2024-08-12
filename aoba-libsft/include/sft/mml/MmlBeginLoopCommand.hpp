#pragma once

#include "MmlCommand.hpp"

namespace Sft {

class MmlBeginLoopCommand : public MmlCommand {
public:
  //! Constructor
  MmlBeginLoopCommand(const MmlParserInfo &info) : MmlCommand(info) {}

  //! Returns the type
  MmlCommandType type() const override { return MmlCommandType::BEGIN_LOOP; }

  //! Updates the execute state
  void updateExecuteState(Song *song, MmlExecutedState &state) const override;
};

}