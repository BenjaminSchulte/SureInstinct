#include <sft/mml/MmlSetInstrumentCommand.hpp>
#include <sft/mml/MmlExecutedScript.hpp>
#include <sft/instrument/InstrumentList.hpp>
#include <sft/song/Song.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
void MmlSetInstrumentCommand::updateExecuteState(Song *song, MmlExecutedState &state) const {
  state.instrument = song->instruments().findById(mInstrument);

  if (state.instrument) {
    state.adsrA = state.instrument->adsr().a;
    state.adsrD = state.instrument->adsr().d;
    state.adsrS = state.instrument->adsr().s;
    state.adsrR = state.instrument->adsr().r;
  }
}
