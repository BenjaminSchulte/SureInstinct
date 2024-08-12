#include <QDebug>
#include <sft/mml/MmlExecutedScript.hpp>
#include <sft/mml/MmlSetTempoCommand.hpp>
#include <sft/mml/MmlParser.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
int MmlSetTempoCommand::bpm() const {
  return MmlParser::convertTempoToBpm(mTempo) + 0.5;
}

// -----------------------------------------------------------------------------
void MmlSetTempoCommand::updateExecuteState(Song *, MmlExecutedState &state) const {
  state.tempo = mTempo;
  state.tempoUpdateTick = state.tick;
}