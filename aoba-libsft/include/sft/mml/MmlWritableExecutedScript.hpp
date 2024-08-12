#pragma once

#include "MmlExecutedScript.hpp"

namespace Sft {
class MmlPlayNoteCommand;

class MmlWritableExecutedScript : public MmlExecutedScript {
public:
  //! Adds a command
  void add(const MmlExecutedCommand &command);

private:
  //! Processes a note
  void processPlayNote(const QSharedPointer<MmlPlayNoteCommand> &note, const MmlExecutedBasicState &state);
};

}