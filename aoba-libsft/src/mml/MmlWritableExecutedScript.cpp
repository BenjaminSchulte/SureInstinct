#include <QDebug>
#include <sft/mml/MmlWritableExecutedScript.hpp>
#include <sft/mml/MmlPlayNoteCommand.hpp>
#include <sft/Configuration.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
void MmlWritableExecutedScript::add(const MmlExecutedCommand &command) {
  mCommands.push_back(command);
  mDuration = command.state().tick + command.command()->duration();

  switch (command.command()->type()) {
    case MmlCommandType::PLAY_NOTE:
      processPlayNote(command.command().dynamicCast<MmlPlayNoteCommand>(), command.state());
      break;  

    default:
      break;
  }
}

// -----------------------------------------------------------------------------
void MmlWritableExecutedScript::processPlayNote(const QSharedPointer<MmlPlayNoteCommand> &note, const MmlExecutedBasicState &state) {
  uint8_t totalNote = note->note() + state.octave * NUM_NOTES_PER_OCTAVE;

  mHighestNote = qMax(mHighestNote, totalNote);
  mLowestNote = qMin(mLowestNote, totalNote);
}