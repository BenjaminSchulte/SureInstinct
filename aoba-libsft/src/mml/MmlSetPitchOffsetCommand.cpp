#include <sft/mml/MmlSetPitchOffsetCommand.hpp>
#include <sft/mml/MmlExecutedScript.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
void MmlSetPitchOffsetCommand::updateExecuteState(Song *, MmlExecutedState &state) const {
  if (mRelative) {
    state.pitchOffset += mNotes;
  } else {
    state.pitchOffset = mNotes;
  }
}
