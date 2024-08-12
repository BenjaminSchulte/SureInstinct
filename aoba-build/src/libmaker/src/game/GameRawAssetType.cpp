#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameRawAssetType.hpp>
#include <aoba/raw/RawScriptAssetResolver.hpp>

using namespace Maker;
using namespace Aoba;


// -----------------------------------------------------------------------------
Aoba::RawAsset *GameRawAsset::configure(const GameConfigNode &config) {
  Aoba::RawAssetSet *instruments = dynamic_cast<Aoba::RawAssetSet*>(mType->assetSet());

  return new Aoba::RawAsset(instruments, mId, config.asDirectAssetFileName("raw", ".bin"));
}


// -----------------------------------------------------------------------------
RawAssetSet *GameRawAssetType::configure(const GameConfigNode &config) {
  RawAssetSet *set = new RawAssetSet(mProject->project(), config["bank"].asInt(-1));
  return set;
}

// -----------------------------------------------------------------------------
Aoba::ScriptAssetResolver *GameRawAssetType::createAssetResolver() const {
  return new RawScriptAssetResolver(dynamic_cast<RawAssetSet*>(mAssetSet));
}

// -----------------------------------------------------------------------------
GameRawAsset *GameRawAssetType::create(const QString &id) const {
  return new GameRawAsset(this, mProject, id);
}

// -----------------------------------------------------------------------------
