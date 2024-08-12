#pragma once

#include "MmlCommand.hpp"

namespace Sft {

class MmlSetPitchChannelEnabledCommand : public MmlCommand {
public:
  //! Constructor
  MmlSetPitchChannelEnabledCommand(const MmlParserInfo &info, bool enabled) : MmlCommand(info), mPitchChannelEnabled(enabled) {}

  //! Returns the type
  MmlCommandType type() const override { return MmlCommandType::SET_PITCH_CHANNEL_ENABLED; }

  //! Returns the enabled
  inline bool enabled() const { return mPitchChannelEnabled; }

  //! Updates the execute state
  void updateExecuteState(Song *song, MmlExecutedState &state) const override;

private:
  //! The enabled size
  bool mPitchChannelEnabled;
};

}