#include <sft/mml/MmlSetAdsrPropertyCommand.hpp>
#include <sft/mml/MmlExecutedScript.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
void MmlSetAdsrPropertyCommand::updateExecuteState(Song *, MmlExecutedState &state) const {
  switch (mProperty) {
    case A: state.adsrA = mValue; break;
    case D: state.adsrD = mValue; break;
    case S: state.adsrS = mValue; break;
    case R: state.adsrR = mValue; break;
  }
}
