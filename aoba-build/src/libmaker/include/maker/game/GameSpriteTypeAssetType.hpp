#pragma once

#include <aoba/sprite/SpriteType.hpp>
#include "GameAssetType.hpp"

namespace Maker {

class GameSpriteTypeAsset : public GameAsset<Aoba::SpriteType> {
public:
  //! Constructor
  GameSpriteTypeAsset(const GameAssetType<Aoba::SpriteType> *type, GameProject *project, const QString &id)
    : GameAsset<Aoba::SpriteType>(type, project, id)
  {}

  //! Creates the asset
  Aoba::SpriteType *configure(const GameConfigNode &config) override;
};

class GameSpriteTypeAssetType : public GameAssetType<Aoba::SpriteType> {
public:
  //! Loads the sprite
  GameSpriteTypeAsset *create(const QString &id) const override;

protected:
  //! Loads the type configuration
  Aoba::SpriteTypeAssetSet *configure(const GameConfigNode &node) override;
  
};

}