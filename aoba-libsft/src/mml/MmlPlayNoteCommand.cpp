#include <QDebug>
#include <sft/mml/MmlExecutedScript.hpp>
#include <sft/mml/MmlPlayNoteCommand.hpp>
#include <sft/mml/MmlParser.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
void MmlPlayNoteCommand::updateExecuteState(Song *, MmlExecutedState &state) const {
  state.pitch = MmlPitchInfo();
}