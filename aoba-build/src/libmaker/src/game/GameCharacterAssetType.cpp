#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameCharacterAssetType.hpp>
#include <maker/game/GamePaletteAssetType.hpp>
#include <maker/game/GameSpriteAssetType.hpp>
#include <aoba/character/CharacterSpriteComponent.hpp>
#include <aoba/character/CharacterFlagsComponent.hpp>
#include <aoba/character/CharacterScriptAssetResolver.hpp>

using namespace Maker;
using namespace Aoba;


// -----------------------------------------------------------------------------
Character *GameCharacterAsset::configure(const GameConfigNode &config) {
  CharacterAssetSet *characters = dynamic_cast<CharacterAssetSet*>(mType->assetSet());

  return new Character(characters, mId, config.asAssetPath("characters"));
}

// -----------------------------------------------------------------------------
GameSpriteAsset *GameCharacterAsset::sprite() const {
  CharacterSpriteComponent *sprites = mAsset->assetSet()->getComponent<CharacterSpriteComponent>();
  if (!sprites) {
    return nullptr;
  }

  Aoba::Sprite *sprite = sprites->getSpriteFor(mAsset);
  if (!sprite) {
    return nullptr;
  }

  return dynamic_cast<GameSpriteAsset*>(mProject->assets().getAsset(sprite->assetSet()->typeId(), sprite->id()));
}


// -----------------------------------------------------------------------------
GameCharacterAssetType::~GameCharacterAssetType() {
}

// -----------------------------------------------------------------------------
CharacterAssetSet *GameCharacterAssetType::configure(const GameConfigNode &config) {
  CharacterAssetSet *set = new CharacterAssetSet(mProject->project(), config["id"].asString(), config["header_rom_bank"].asInt(0xC0));

  for (const GameConfigNode &comp : config["components"].asList()) {
    QString type(comp["component"].asString());

    if (type == "sprite" && loadSpriteComponent(set, comp)) {
    } else if (type == "flags" && loadFlagsComponent(set, comp)) {
    } else {
      mProject->log().error("Could not load character component: " + type);
      delete set;
      return nullptr;
    }
  }

  return set;
}

// -----------------------------------------------------------------------------
Aoba::ScriptAssetResolver *GameCharacterAssetType::createAssetResolver() const {
  return new CharacterScriptAssetResolver(dynamic_cast<CharacterAssetSet*>(mAssetSet));
}

// -----------------------------------------------------------------------------
bool GameCharacterAssetType::loadFlagsComponent(CharacterAssetSet *character, const GameConfigNode &config) {
  CharacterComponent *comp = new CharacterFlagsComponent(config["id"].asString());
  character->addComponent(comp);
  mComponents.push_back(comp);
  return true;
}

// -----------------------------------------------------------------------------
bool GameCharacterAssetType::loadSpriteComponent(CharacterAssetSet *character, const GameConfigNode &config) {
  QString spriteId = config["sprite"].asString("");
  GameSpriteAssetType *sprites = mProject->assetTypes().get<GameSpriteAssetType>(spriteId);
  if (!sprites) {
    mProject->log().error("Could not find sprite type " + spriteId);
    return false;
  }

  SpriteAssetSet *spriteSet = dynamic_cast<SpriteAssetSet*>(sprites->assetSet());

  CharacterComponent *comp = new CharacterSpriteComponent(config["id"].asString(), spriteSet);
  character->addComponent(comp);
  mComponents.push_back(comp);
  return true;
}

// -----------------------------------------------------------------------------
GameCharacterAsset *GameCharacterAssetType::create(const QString &id) const {
  return new GameCharacterAsset(this, mProject, id);
}

// -----------------------------------------------------------------------------
