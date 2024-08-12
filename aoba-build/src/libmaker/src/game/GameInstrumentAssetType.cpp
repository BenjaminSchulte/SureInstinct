#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameInstrumentAssetType.hpp>
#include <maker/game/GameSampleAssetType.hpp>

using namespace Maker;
using namespace Aoba;


// -----------------------------------------------------------------------------
Aoba::Instrument *GameInstrumentAsset::configure(const GameConfigNode &config) {
  Aoba::InstrumentAssetSet *instruments = dynamic_cast<Aoba::InstrumentAssetSet*>(mType->assetSet());

  return new Aoba::Instrument(instruments, mId, config.asAssetFileName("instruments", "instrument.yml"));
}


// -----------------------------------------------------------------------------
InstrumentAssetSet *GameInstrumentAssetType::configure(const GameConfigNode &config) {
  QString sampleId = config["samples"].asString("");
  GameSampleAssetType *samples = mProject->assetTypes().get<GameSampleAssetType>(sampleId);
  if (!samples) {
    mProject->log().error("Could not find sample type " + sampleId);
    return nullptr;
  }

  uint8_t bank = config["instrument_bank"].asInt(0xC0);

  SampleAssetSet *sampleSet = dynamic_cast<SampleAssetSet*>(samples->assetSet());
  InstrumentAssetSet *set = new InstrumentAssetSet(mProject->project(), sampleSet, bank);
  return set;
}

// -----------------------------------------------------------------------------
GameInstrumentAsset *GameInstrumentAssetType::create(const QString &id) const {
  return new GameInstrumentAsset(this, mProject, id);
}

// -----------------------------------------------------------------------------
