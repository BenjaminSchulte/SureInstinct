#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameSfxAssetType.hpp>
#include <maker/game/GameInstrumentAssetType.hpp>
#include <aoba/sfx/SfxScriptAssetResolver.hpp>

using namespace Maker;
using namespace Aoba;


// -----------------------------------------------------------------------------
Aoba::Sfx *GameSfxAsset::configure(const GameConfigNode &config) {
  Aoba::SfxAssetSet *instruments = dynamic_cast<Aoba::SfxAssetSet*>(mType->assetSet());

  return new Aoba::Sfx(instruments, mId, config.asAssetFileName("sfx", "sfx.mml"));
}


// -----------------------------------------------------------------------------
SfxAssetSet *GameSfxAssetType::configure(const GameConfigNode &config) {
  QString instrumentId = config["instruments"].asString("");
  GameInstrumentAssetType *instruments = mProject->assetTypes().get<GameInstrumentAssetType>(instrumentId);
  if (!instruments) {
    mProject->log().error("Could not find instrument type " + instrumentId);
    return nullptr;
  }

  InstrumentAssetSet *instrumentSet = dynamic_cast<InstrumentAssetSet*>(instruments->assetSet());
  SfxAssetSet *set = new SfxAssetSet(mProject->project(), instrumentSet);
  return set;
}

// -----------------------------------------------------------------------------
Aoba::ScriptAssetResolver *GameSfxAssetType::createAssetResolver() const {
  return new SfxScriptAssetResolver(dynamic_cast<SfxAssetSet*>(mAssetSet));
}

// -----------------------------------------------------------------------------
GameSfxAsset *GameSfxAssetType::create(const QString &id) const {
  return new GameSfxAsset(this, mProject, id);
}

// -----------------------------------------------------------------------------
