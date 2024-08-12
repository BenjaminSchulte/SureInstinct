#pragma once

#include "MmlCommand.hpp"

namespace Sft {

class MmlPauseNoteCommand : public MmlCommand {
public:
  //! Constructor
  MmlPauseNoteCommand(const MmlParserInfo &info, unsigned duration) : MmlCommand(info), mDuration(duration) {}

  //! Returns the type
  MmlCommandType type() const override { return MmlCommandType::PAUSE; }

  //! Returns the duration
  unsigned duration() const override { return mDuration; }

private:
  //! The duration size
  unsigned mDuration;
};

}