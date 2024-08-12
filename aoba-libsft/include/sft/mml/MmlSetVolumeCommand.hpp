#pragma once

#include "MmlCommand.hpp"

namespace Sft {

class MmlSetVolumeCommand : public MmlCommand {
public:
  //! Constructor
  MmlSetVolumeCommand(const MmlParserInfo &info, int volume) : MmlCommand(info), mVolume(volume) {}

  //! Returns the type
  MmlCommandType type() const override { return MmlCommandType::SET_VOLUME; }

  //! Returns the volume
  inline int volume() const { return mVolume; }

  //! Updates the execute state
  void updateExecuteState(Song *song, MmlExecutedState &state) const override;

private:
  //! The volume size
  int mVolume;
};

}