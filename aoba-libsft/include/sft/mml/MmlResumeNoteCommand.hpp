#pragma once

#include "MmlCommand.hpp"

namespace Sft {

class MmlResumeNoteCommand : public MmlCommand {
public:
  //! Constructor
  MmlResumeNoteCommand(const MmlParserInfo &info, unsigned duration) : MmlCommand(info), mDuration(duration) {}

  //! Returns the type
  MmlCommandType type() const override { return MmlCommandType::RESUME_NOTE; }

  //! Returns the duration
  unsigned duration() const override { return mDuration; }

private:
  //! The duration size
  unsigned mDuration;
};

}