#pragma once

#include <aoba/image/Image.hpp>
#include <aoba/image/ImageConfig.hpp>
#include "GameAssetType.hpp"

namespace Maker {

class GameImageAsset : public GameAsset<Aoba::BaseImage> {
public:
  //! Constructor
  GameImageAsset(const GameAssetType<Aoba::BaseImage> *type, GameProject *project, const QString &id)
    : GameAsset<Aoba::BaseImage>(type, project, id)
  {}

  //! Creates the asset
  Aoba::Image *configure(const GameConfigNode &config) override;
};

class GameImageAssetType : public GameAssetType<Aoba::BaseImage> {
public:
  //! Loads the sprite
  GameImageAsset *create(const QString &id) const override;

protected:
  //! Loads the type configuration
  Aoba::ImageAssetSet *configure(const GameConfigNode &node) override;

  //! Creates the asset resolver
  Aoba::ScriptAssetResolver *createAssetResolver() const override;
  
};

}