#include <aoba/log/Log.hpp>
#include <aoba/character/CharacterScriptAssetResolver.hpp>
#include <aoba/character/Character.hpp>
#include <aoba/character/CharacterComponent.hpp>
#include <aoba/character/CharacterSpriteComponent.hpp>
#include <aoba/sprite/Sprite.hpp>
#include <aoba/sprite/GeneratedSpriteAnimation.hpp>
#include <aoba/sprite/SpriteType.hpp>
#include <aoba/sprite/SpriteAnimationIndex.hpp>
#include <fma/core/Number.hpp>
#include <fma/core/SymbolReference.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <fma/symbol/SymbolReference.hpp>

using namespace Aoba;
using namespace FMA::symbol;
using namespace FMA::core;
using namespace FMA::interpret;

// ----------------------------------------------------------------------------
QVariant CharacterScriptAssetResolver::resolve(const QString &id, const QString &type) {
  QStringList parts = id.split('/');
  QString characterId = parts[0];
  QString otherId = parts.size() > 1 ? parts[1] : "";

  Character *character = mType->get(characterId);
  if (!character) {
    Aoba::log::error("Unable to find character " + id);
    return QVariant();
  }

  if (type == "address") {
    return character->symbolName();
  } else if (type == "animation_id" || type == "direction_id") {
    return resolveSprite(character, otherId, type);
  } else {
    Aoba::log::error("Unsupported character type: " + type);
    return QVariant();
  }
}

// ----------------------------------------------------------------------------
QVariant CharacterScriptAssetResolver::resolveSprite(Character *character, const QString &id, const QString &type) {
  CharacterSpriteComponent *spriteComponent = nullptr;

  for (CharacterComponent *component : character->assetSet()->components()) {
    if (component->typeId() != "CharacterSpriteComponent") {
      continue;
    }

    spriteComponent = dynamic_cast<CharacterSpriteComponent*>(component);
    break;
  }

  if (!spriteComponent) {
    Aoba::log::error("Can not find Sprite component on character");
    return QVariant();
  }

  Sprite *sprite = spriteComponent->sprites()->get(character->properties()["sprite"].toInt());

  if (type == "animation_id") {
    int index = sprite->generatedAnimation(id)->animationId();
    if (index < 0) {
      Aoba::log::error("Can not find animation " + id + " for character " + character->id());
    }
    return index;
  } else if (type == "direction_id") {
    return sprite->spriteType()->animationIndex()->directionId(id);
  }

  return QVariant();
}
