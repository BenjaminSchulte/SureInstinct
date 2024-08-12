#pragma once

#include "MmlCommand.hpp"

namespace Sft {

class MmlSetNoiseEnabledCommand : public MmlCommand {
public:
  //! Constructor
  MmlSetNoiseEnabledCommand(const MmlParserInfo &info, bool enabled, int power) : MmlCommand(info), mNoiseEnabled(enabled), mPower(power) {}

  //! Returns the type
  MmlCommandType type() const override { return MmlCommandType::SET_NOISE_ENABLED; }

  //! Returns the enabled
  inline bool enabled() const { return mNoiseEnabled; }

  //! Returns the enabled
  inline int power() const { return mPower; }

  //! Updates the execute state
  void updateExecuteState(Song *song, MmlExecutedState &state) const override;

private:
  //! The enabled size
  bool mNoiseEnabled;

  //! The power
  int mPower;
};

}