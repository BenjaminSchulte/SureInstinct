#pragma once

#include "MmlCommand.hpp"

namespace Sft {

class MmlSetTempoCommand : public MmlCommand {
public:
  //! Constructor
  MmlSetTempoCommand(const MmlParserInfo &info, int tempo) : MmlCommand(info), mTempo(tempo) {}

  //! Returns the type
  MmlCommandType type() const override { return MmlCommandType::SET_TEMPO; }

  //! Returns the tempo
  inline int tempo() const { return mTempo; }

  //! Returns the tempo in BPM
  int bpm() const;

  //! Updates the execute state
  void updateExecuteState(Song *song, MmlExecutedState &state) const override;

private:
  //! The tempo size
  int mTempo;
};

}