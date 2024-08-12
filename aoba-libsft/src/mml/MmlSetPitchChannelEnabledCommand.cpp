#include <sft/mml/MmlSetPitchChannelEnabledCommand.hpp>
#include <sft/mml/MmlExecutedScript.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
void MmlSetPitchChannelEnabledCommand::updateExecuteState(Song *, MmlExecutedState &state) const {
  state.pitchTrackEnabled = mPitchChannelEnabled;
}
