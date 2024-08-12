#include <aoba/character/Character.hpp>
#include <aoba/character/CharacterSpriteComponent.hpp>
#include <aoba/sprite/Sprite.hpp>
#include <aoba/palette/PaletteGroup.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool CharacterSpriteComponent::load(Character *character, YAML::Node &config) const {
  QString spriteId = Yaml::asString(config[id().toStdString()]);
  if (spriteId.isEmpty()) {
    return loadEmptySprite(character, config);
  } else {
    return loadSprite(spriteId, character, config);
  }
}

// -----------------------------------------------------------------------------
bool CharacterSpriteComponent::loadEmptySprite(Character *character, YAML::Node &) const {
  character->properties().insert(mId + ".palette", "");
  character->properties().insert(mId + ".flags", 0x06);
  character->properties().insert(mId, 0);
  return true;
}

// -----------------------------------------------------------------------------
bool CharacterSpriteComponent::loadSprite(const QString &spriteId, Character *character, YAML::Node &config) const {
  int32_t assetId = mSprites->assetId(spriteId);
  if (assetId == -1) {
    Aoba::log::error("Could not find any sprite with the ID: " + spriteId);
    return false;
  }

  uint8_t flags = 0;
  if (Yaml::asBool(config[(id() + "_dedicated_palette_slot").toStdString()], false)) {
    flags |= 1;
  }


  Sprite *sprite = mSprites->get(spriteId);
  character->properties().insert(mId + ".palette", sprite->mainPaletteGroup()->assetSymbolName());
  character->properties().insert(mId + ".flags", (int)flags);
  character->properties().insert(mId + ".name", spriteId);
  character->properties().insert(mId, (int)assetId);
  return true;
}

// -----------------------------------------------------------------------------
bool CharacterSpriteComponent::build(Character *character, FMA::linker::LinkerBlock *block) const {
  QString paletteId = character->properties()[mId + ".palette"].toString();

  block->writeNumber(character->properties()[mId].toInt(), 2);
  if (paletteId.isEmpty()) {
    block->writeNumber(0, 2);
  } else {
    block->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(paletteId.toStdString())), 2);
  }
  block->writeNumber(character->properties()[mId + ".flags"].toInt(), 1); // flags
  return true;
}

// -----------------------------------------------------------------------------
Sprite *CharacterSpriteComponent::getSpriteFor(Character *character) const {
  const QString &id = character->properties()[mId + ".name"].toString();
  return mSprites->get(id);
}
