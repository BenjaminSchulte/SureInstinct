#pragma once

#include <aoba/character/Character.hpp>
#include <aoba/character/CharacterComponent.hpp>
#include "GameAsset.hpp"

namespace Maker {

class GameSpriteAsset;

class GameCharacterAsset : public GameAsset<Aoba::Character> {
public:
  //! Constructor
  GameCharacterAsset(const GameAssetType<Aoba::Character> *type, GameProject *project, const QString &id)
    : GameAsset<Aoba::Character>(type, project, id)
  {}

  //! Creates the asset
  Aoba::Character *configure(const GameConfigNode &config) override;

  //! Returns the sprite
  GameSpriteAsset *sprite() const;
};

class GameCharacterAssetType : public GameAssetType<Aoba::Character> {
public:
  //! Deconstructor
  ~GameCharacterAssetType();

  //! Loads the character
  GameCharacterAsset *create(const QString &id) const override;

protected:
  //! Loads the type configuration
  Aoba::CharacterAssetSet *configure(const GameConfigNode &node) override;

  //! Creates the asset resolver
  Aoba::ScriptAssetResolver *createAssetResolver() const override;

  //! Loads the sprite component
  bool loadSpriteComponent(Aoba::CharacterAssetSet *character, const GameConfigNode &config);

  //! Loads the sprite component
  bool loadFlagsComponent(Aoba::CharacterAssetSet *character, const GameConfigNode &config);
  
  //! List of all components
  QVector<Aoba::CharacterComponent*> mComponents;
};

}