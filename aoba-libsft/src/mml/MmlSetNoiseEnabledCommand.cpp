#include <sft/mml/MmlSetNoiseEnabledCommand.hpp>
#include <sft/mml/MmlExecutedScript.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
void MmlSetNoiseEnabledCommand::updateExecuteState(Song *, MmlExecutedState &state) const {
  state.noiseEnabled = mNoiseEnabled;
  state.noiseClock = mPower;
}
