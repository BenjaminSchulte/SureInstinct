#pragma once

#include <aoba/song/SongBuilderAdapter.hpp>
#include <aoba/instrument/Instrument.hpp>

namespace Aoba {
class InstrumentAssetSet;

class AobaSongBuilderAdapter : public SongBuilderAdapter {
public:
  //! Constructor
  AobaSongBuilderAdapter(InstrumentAssetSet *instruments) : mInstruments(instruments) {}

  //! Resolves the instrument ID (-1 on error)
  int16_t resolveInstrumentId(const QString &id) const {
    return mInstruments->assetId(id);
  }

private:
  //! Instruments
  InstrumentAssetSet *mInstruments;
};

}
