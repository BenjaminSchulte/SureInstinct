#pragma once

#include <QRect>
#include "./CharacterComponent.hpp"

namespace Aoba {
class Sprite;
class SpriteAssetSet;

struct CharacterFlags {
  bool mNpcCollidableGroup = false;
  bool mPlayerCollidableGroup = false;
  bool mCanMoveOther = false;
  bool mCanBeMoved = true;
  bool mIsFlying = false;
  bool mCollides = true;
  bool mIsEnterTrigger = false;
  bool mExitIsBlocking = true;
  bool mNoMovement = false;
  uint8_t mIndexZ = 0x80;
  int mCollisionBoxId;
  int mHitBoxId;
};

class CharacterFlagsComponent : public CharacterComponent {
public:
  //! Constructor
  CharacterFlagsComponent(const QString &id)
    : CharacterComponent(id)
  {}

  //! Returns the type ID
  static QString TypeId() { return "CharacterFlagsComponent"; }

  //! Returns the type ID
  QString typeId() const override { return TypeId(); }

  //! Loads the component
  bool load(Character *level, YAML::Node &config) const override;

  //! Builds the component
  bool build(Character*, FMA::linker::LinkerBlock *block) const override;
};
  
}

Q_DECLARE_METATYPE(Aoba::CharacterFlags);