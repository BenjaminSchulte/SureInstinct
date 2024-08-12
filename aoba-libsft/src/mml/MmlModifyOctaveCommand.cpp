#include <sft/mml/MmlModifyOctaveCommand.hpp>
#include <sft/mml/MmlExecutedScript.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
void MmlModifyOctaveCommand::updateExecuteState(Song *, MmlExecutedState &state) const {
  int newOctave = (int)state.octave + mStep;

  if (newOctave < FIRST_OCTAVE) {
    newOctave = FIRST_OCTAVE;
  } else if (newOctave > LAST_OCTAVE) {
    newOctave = LAST_OCTAVE;
  }

  state.octave = newOctave;
}