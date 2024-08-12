#pragma once

#include "./CharacterComponent.hpp"

namespace Aoba {
class Sprite;
class SpriteAssetSet;

class CharacterSpriteComponent : public CharacterComponent {
public:
  //! Constructor
  CharacterSpriteComponent(const QString &id, SpriteAssetSet *palettes)
    : CharacterComponent(id)
    , mSprites(palettes)
  {}

  //! Returns the type ID
  static QString TypeId() { return "CharacterSpriteComponent"; }

  //! Returns the type ID
  QString typeId() const override { return TypeId(); }

  //! Loads the component
  bool load(Character *character, YAML::Node &config) const override;

  //! Builds the component
  bool build(Character*, FMA::linker::LinkerBlock *block) const override;

  //! Loads the component
  Sprite *getSpriteFor(Character *character) const;

  //! Returns the sprite set
  inline SpriteAssetSet *sprites() const { return mSprites; }

private:
  //! Loads an empty sprite
  bool loadEmptySprite(Character *character, YAML::Node &config) const;

  //! Loads the component
  bool loadSprite(const QString &id, Character *character, YAML::Node &config) const;
  
  //! Sprites
  SpriteAssetSet *mSprites;
};
  
}