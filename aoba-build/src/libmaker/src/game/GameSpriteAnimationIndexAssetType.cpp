#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameLinkResult.hpp>
#include <maker/game/GamePaletteGroupAssetType.hpp>
#include <maker/game/GameSpriteAnimationIndexAssetType.hpp>
#include <aoba/sprite/AnimationIndexScriptAssetResolver.hpp>

using namespace Maker;
using namespace Aoba;


// -----------------------------------------------------------------------------
Aoba::SpriteAnimationIndex *GameSpriteAnimationIndexAsset::configure(const GameConfigNode &) {
  return nullptr;
}

// -----------------------------------------------------------------------------
QStringList GameSpriteAnimationIndexAssetType::dependencies(const GameConfigNode &config) const {
  QString animationIndexId = config["extends"].asString("");

  if (animationIndexId.isEmpty()) {
    return {};
  } else {
    return {animationIndexId};
  }
}

// -----------------------------------------------------------------------------
SpriteAnimationIndexAssetSet *GameSpriteAnimationIndexAssetType::configure(const GameConfigNode &config) {
  QString animationIndexId = config["extends"].asString("");
  SpriteAnimationIndexAssetSet *parent = nullptr;

  if (!animationIndexId.isEmpty()) {
    GameSpriteAnimationIndexAssetType *animationIndex = mProject->assetTypes().get<GameSpriteAnimationIndexAssetType>(animationIndexId);
    if (!animationIndex) {
      mProject->log().error("Could not find animation index type " + animationIndexId);
      return nullptr;
    }
  
    parent = dynamic_cast<SpriteAnimationIndexAssetSet*>(animationIndex->assetSet());
  }
  
  int animationIndexBank = config["animation_script_bank"].asInt(0xC0);
  int frameReferenceBank = config["frame_reference_bank"].asInt(0xC0);

  SpriteAnimationIndexAssetSet *set = new SpriteAnimationIndexAssetSet(mProject->project(), parent, animationIndexBank, frameReferenceBank);
  for (const auto &item : config["directions"].asList()) {
    set->addDirection(item.asString());
  }
  for (const auto &item : config["animations"].asList()) {
    set->addAnimation(item.asString());
  }
  for (const auto &item : config["frame_groups"].asList()) {
    set->addFrameGroup(item.asString());
  }

  return set;
}
  
// -----------------------------------------------------------------------------
bool GameSpriteAnimationIndexAssetType::linkType(GameLinkResult &result) const {
  auto *set = dynamic_cast<Aoba::SpriteAnimationIndexAssetSet*>(assetSet());

  for (const QString &obj : set->assetLinkerObject().getFmaObjectFiles()) {
    result.addObject(obj);
  }

  return true;
}

// -----------------------------------------------------------------------------
GameSpriteAnimationIndexAsset *GameSpriteAnimationIndexAssetType::create(const QString &id) const {
  return new GameSpriteAnimationIndexAsset(this, mProject, id);
}

// -----------------------------------------------------------------------------
Aoba::ScriptAssetResolver *GameSpriteAnimationIndexAssetType::createAssetResolver() const {
  return new AnimationIndexScriptAssetResolver(dynamic_cast<SpriteAnimationIndexAssetSet*>(mAssetSet));
}


// -----------------------------------------------------------------------------
