#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameSfxAssetType.hpp>
#include <maker/game/GameSfxLibraryAssetType.hpp>
#include <aoba/sfx/SfxLibraryScriptAssetResolver.hpp>

using namespace Maker;
using namespace Aoba;


// -----------------------------------------------------------------------------
Aoba::SfxLibrary *GameSfxLibraryAsset::configure(const GameConfigNode &config) {
  Aoba::SfxLibraryAssetSet *instruments = dynamic_cast<Aoba::SfxLibraryAssetSet*>(mType->assetSet());

  return new Aoba::SfxLibrary(instruments, mId, config.asDirectAssetFileName("sfxlib", ".yml"));
}


// -----------------------------------------------------------------------------
SfxLibraryAssetSet *GameSfxLibraryAssetType::configure(const GameConfigNode &config) {
  QString sfxId = config["sfx"].asString("");
  GameSfxAssetType *sfxs = mProject->assetTypes().get<GameSfxAssetType>(sfxId);
  if (!sfxs) {
    mProject->log().error("Could not find sfx type " + sfxId);
    return nullptr;
  }

  SfxAssetSet *sfxSet = dynamic_cast<SfxAssetSet*>(sfxs->assetSet());
  SfxLibraryAssetSet *set = new SfxLibraryAssetSet(mProject->project(), sfxSet);
  return set;
}

// -----------------------------------------------------------------------------
Aoba::ScriptAssetResolver *GameSfxLibraryAssetType::createAssetResolver() const {
  return new SfxLibraryScriptAssetResolver(dynamic_cast<SfxLibraryAssetSet*>(mAssetSet));
}

// -----------------------------------------------------------------------------
GameSfxLibraryAsset *GameSfxLibraryAssetType::create(const QString &id) const {
  return new GameSfxLibraryAsset(this, mProject, id);
}

// -----------------------------------------------------------------------------
