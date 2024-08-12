#include <sft/mml/MmlSetOctaveCommand.hpp>
#include <sft/mml/MmlExecutedScript.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
void MmlSetOctaveCommand::updateExecuteState(Song *, MmlExecutedState &state) const {
  int target = mTarget - 1;

  if (target < 0) {
    target = 0;
  } else if (target >= NUM_OCTAVES) {
    target = NUM_OCTAVES - 1;
  }
  
  state.octave = target;
}
