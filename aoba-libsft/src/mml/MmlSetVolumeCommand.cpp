#include <sft/mml/MmlSetVolumeCommand.hpp>
#include <sft/mml/MmlExecutedScript.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
void MmlSetVolumeCommand::updateExecuteState(Song *, MmlExecutedState &state) const {
  state.volume = mVolume;
}
