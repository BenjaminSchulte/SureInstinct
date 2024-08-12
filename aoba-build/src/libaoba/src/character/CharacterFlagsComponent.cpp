#include <QDebug>
#include <aoba/character/Character.hpp>
#include <aoba/character/CharacterFlagsComponent.hpp>
#include <aoba/palette/PaletteGroup.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool CharacterFlagsComponent::load(Character *character, YAML::Node &globalConfig) const {
  const YAML::Node &config = globalConfig[mId.toStdString()];

  CharacterFlags flags;
  QString group = Yaml::asString(config["collision_group"], "npc");
  if (group == "npc") {
    flags.mNpcCollidableGroup = true;
  } else if (group == "player") {
    flags.mPlayerCollidableGroup = true;
  } else if (group == "all") {
    flags.mNpcCollidableGroup = true;
    flags.mPlayerCollidableGroup = true;
  } else if (group == "none") {
  } else {
    Aoba::log::error("Unsupported collision group for character: " + group);
    return false;
  }

  QString indexZ = Yaml::asString(config["render_index"], "default");
  if (indexZ == "background") {
    flags.mIndexZ = 0x40;
  } else if (indexZ == "foreground") {
    flags.mIndexZ = 0xC0;
  } else if (indexZ == "default") {
  } else if (indexZ == "least") {
    flags.mIndexZ = 0x00;
  } else {
    Aoba::log::error("Unsupported render_index: " + indexZ);
    return false;
  }

  flags.mCanBeMoved = Yaml::asBool(config["can_be_pushed"], true);
  flags.mCanMoveOther = Yaml::asBool(config["can_push_others"], false);
  flags.mIsFlying = Yaml::asBool(config["flying"], false);
  flags.mCollides = Yaml::asBool(config["collides"], true);
  flags.mIsEnterTrigger = Yaml::asBool(config["is_enter_trigger"], false);
  flags.mExitIsBlocking = Yaml::asBool(config["exit_is_blocking"], true);
  flags.mNoMovement = !Yaml::asBool(config["moves"], true);

  QRect collisionBox(-8, -8, 16, 2);
  if (Yaml::isObject(config["collisionbox"])) {
    collisionBox = QRect(QPoint(
      Yaml::asInt(config["collisionbox"]["left"], -8),
      Yaml::asInt(config["collisionbox"]["top"], -8)
    ), QPoint(
      Yaml::asInt(config["collisionbox"]["right"], 16),
      Yaml::asInt(config["collisionbox"]["bottom"], 2)
    ));
  }

  QRect hitBox(-14, -14, 29, 21);
  if (Yaml::isObject(config["hitbox"])) {
    hitBox = QRect(QPoint(
      Yaml::asInt(config["hitbox"]["left"], -14),
      Yaml::asInt(config["hitbox"]["top"], -14)
    ), QPoint(
      Yaml::asInt(config["hitbox"]["right"], 14),
      Yaml::asInt(config["hitbox"]["bottom"], 6)
    ));
  }

  flags.mCollisionBoxId = character->assetSet()->getHitBoxId(collisionBox);
  flags.mHitBoxId = character->assetSet()->getHitBoxId(hitBox);

  QVariant value;
  value.setValue(flags);
  character->properties().insert(mId, value);
  return true;
}

// -----------------------------------------------------------------------------
bool CharacterFlagsComponent::build(Character *character, FMA::linker::LinkerBlock *block) const {
  const CharacterFlags &flags = character->properties()[mId].value<CharacterFlags>();

  // general flags
  block->writeNumber(
    (flags.mNpcCollidableGroup ? 0x01 : 0) |
    (flags.mPlayerCollidableGroup ? 0x02 : 0) |
    (flags.mCanBeMoved ? 0x04 : 0) |
    (flags.mCanMoveOther ? 0x08 : 0) |
    (flags.mIsFlying ? 0x10 : 0) |
    ((flags.mIndexZ >> 1) & 0x60)
    // 0x80
  , 1); // flags

  // collidable_flags
  block->writeNumber(
    (flags.mExitIsBlocking ? 0x01 : 0) |
    (flags.mIsEnterTrigger ? 0x02 : 0) |
    // see collidable.asm
    (flags.mCollides ? 0 : 0x80)
  , 1); // flags

  // basic flags
  block->writeNumber(
    (flags.mNoMovement ? 0x80 : 0)
  , 1); // flags

  block->writeNumber(flags.mHitBoxId, 1);
  block->writeNumber(flags.mCollisionBoxId, 1);
  
  return true;
}