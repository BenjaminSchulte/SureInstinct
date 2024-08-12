#include <QDebug>
#include <sft/mml/MmlBeginLoopCommand.hpp>
#include <sft/mml/MmlExecutedScript.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
void MmlBeginLoopCommand::updateExecuteState(Song *, MmlExecutedState &state) const {
  state.commandIndexStack.push_back(state.commandIndex);
  state.repeatTimeStack.push_back(0);
  state.oldOctaveStack.push_back(state.octave);
}
