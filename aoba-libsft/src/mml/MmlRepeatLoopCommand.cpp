#include <QDebug>
#include <sft/mml/MmlRepeatLoopCommand.hpp>
#include <sft/mml/MmlExecutedScript.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
void MmlRepeatLoopCommand::updateExecuteState(Song *, MmlExecutedState &state) const {
  if (state.repeatTimeStack.isEmpty()) {
    return;
  }

  int newCounter = state.repeatTimeStack.last() + 1;
  if (newCounter < mTimes) {
    state.repeatTimeStack.last() = newCounter;
    state.commandIndex = state.commandIndexStack.back();
    state.octave = state.oldOctaveStack.back();
  } else {
    state.commandIndexStack.pop_back();
    state.repeatTimeStack.pop_back();
    state.oldOctaveStack.pop_back();
  }

}
