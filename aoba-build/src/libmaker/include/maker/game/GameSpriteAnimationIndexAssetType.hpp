#pragma once

#include <aoba/sprite/SpriteAnimationIndex.hpp>
#include "GameAssetType.hpp"

namespace Maker {

class GameSpriteAnimationIndexAsset : public GameAsset<Aoba::SpriteAnimationIndex> {
public:
  //! Constructor
  GameSpriteAnimationIndexAsset(const GameAssetType<Aoba::SpriteAnimationIndex> *type, GameProject *project, const QString &id)
    : GameAsset<Aoba::SpriteAnimationIndex>(type, project, id)
  {}

  //! Creates the asset
  Aoba::SpriteAnimationIndex *configure(const GameConfigNode &config) override;
};

class GameSpriteAnimationIndexAssetType : public GameAssetType<Aoba::SpriteAnimationIndex> {
public:
  //! Loads the sprite
  GameSpriteAnimationIndexAsset *create(const QString &id) const override;

  //! Returns the dependencies
  QStringList dependencies(const GameConfigNode &) const override;
  
  //! Links this type
  bool linkType(GameLinkResult &result) const override;

  //! Creates the asset resolver
  Aoba::ScriptAssetResolver *createAssetResolver() const override;

protected:
  //! Loads the type configuration
  Aoba::SpriteAnimationIndexAssetSet *configure(const GameConfigNode &node) override;
  
};

}