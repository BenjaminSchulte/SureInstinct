#pragma once

#include "MmlCommand.hpp"

namespace Sft {

class MmlPlayNoteCommand : public MmlCommand {
public:
  //! Constructor
  MmlPlayNoteCommand(const MmlParserInfo &info, unsigned note, unsigned duration)
    : MmlCommand(info), mNote(note), mDuration(duration) {}

  //! Returns the type
  MmlCommandType type() const override { return MmlCommandType::PLAY_NOTE; }

  //! Returns the node
  inline unsigned note() const { return mNote; }

  //! Returns the duration
  unsigned duration() const override { return mDuration; }

  //! Updates the execute state
  void updateExecuteState(Song *, MmlExecutedState &) const override;

private:
  //! The note to play
  unsigned mNote;

  //! The duration size
  unsigned mDuration;
};

}