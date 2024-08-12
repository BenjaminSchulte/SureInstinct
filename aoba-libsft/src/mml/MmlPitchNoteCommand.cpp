#include <QDebug>
#include <sft/mml/MmlExecutedScript.hpp>
#include <sft/mml/MmlPitchNoteCommand.hpp>
#include <sft/mml/MmlParser.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
void MmlPitchNoteCommand::updateExecuteState(Song *, MmlExecutedState &state) const {
  state.pitch.duration = mDuration;
  state.pitch.notes = mNotes;
}