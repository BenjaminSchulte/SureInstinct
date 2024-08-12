#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GamePaletteGroupAssetType.hpp>
#include <maker/game/GameSpriteTypeAssetType.hpp>
#include <maker/game/GameSpriteAnimationIndexAssetType.hpp>

using namespace Maker;
using namespace Aoba;


// -----------------------------------------------------------------------------
Aoba::SpriteType *GameSpriteTypeAsset::configure(const GameConfigNode &) {
  return nullptr;
}


// -----------------------------------------------------------------------------
SpriteTypeAssetSet *GameSpriteTypeAssetType::configure(const GameConfigNode &config) {
  QString animationIndexId = config["animation_index"].asString("");
  GameSpriteAnimationIndexAssetType *animationIndex = mProject->assetTypes().get<GameSpriteAnimationIndexAssetType>(animationIndexId);
  if (!animationIndex) {
    mProject->log().error("Could not find animation index type " + animationIndexId);
    return nullptr;
  }
  
  SpriteAnimationIndexAssetSet *animationIndexSet = dynamic_cast<SpriteAnimationIndexAssetSet*>(animationIndex->assetSet());
  SpriteTypeAssetSet *set = new SpriteTypeAssetSet(mProject->project(), mId, animationIndexSet);

  return set;
}

// -----------------------------------------------------------------------------
GameSpriteTypeAsset *GameSpriteTypeAssetType::create(const QString &id) const {
  return new GameSpriteTypeAsset(this, mProject, id);
}

// -----------------------------------------------------------------------------
