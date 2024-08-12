#include <QDebug>
#include <sft/mml/MmlExecutedScript.hpp>
#include <sft/mml/MmlVibratePitchCommand.hpp>
#include <sft/mml/MmlParser.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
void MmlVibratePitchCommand::updateExecuteState(Song *, MmlExecutedState &state) const {
  state.vibrate.duration = mDuration;
  state.vibrate.notes = mNotes;
}