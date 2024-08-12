#pragma once

#include "MmlCommand.hpp"

namespace Sft {

class MmlSetTrackRepeatOffsetCommand : public MmlCommand {
public:
  //! Constructor
  MmlSetTrackRepeatOffsetCommand(const MmlParserInfo &info) : MmlCommand(info) {}

  //! Returns the type
  MmlCommandType type() const override { return MmlCommandType::SET_TRACK_REPEAT_OFFSET; }
};

}