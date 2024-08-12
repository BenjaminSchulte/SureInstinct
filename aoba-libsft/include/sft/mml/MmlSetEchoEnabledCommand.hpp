#pragma once

#include "MmlCommand.hpp"

namespace Sft {

class MmlSetEchoEnabledCommand : public MmlCommand {
public:
  //! Constructor
  MmlSetEchoEnabledCommand(const MmlParserInfo &info, bool enabled) : MmlCommand(info), mEchoEnabled(enabled) {}

  //! Returns the type
  MmlCommandType type() const override { return MmlCommandType::SET_ECHO_ENABLED; }

  //! Returns the enabled
  inline bool enabled() const { return mEchoEnabled; }

private:
  //! The enabled size
  bool mEchoEnabled;
};

}