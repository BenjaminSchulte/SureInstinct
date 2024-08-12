#pragma once

#include <aoba/image/RawImage.hpp>
#include <aoba/image/RawImageConfig.hpp>
#include "GameAssetType.hpp"

namespace Maker {

class GameRawImageAsset : public GameAsset<Aoba::BaseImage> {
public:
  //! Constructor
  GameRawImageAsset(const GameAssetType<Aoba::BaseImage> *type, GameProject *project, const QString &id)
    : GameAsset<Aoba::BaseImage>(type, project, id)
  {}

  //! Creates the asset
  Aoba::RawImage *configure(const GameConfigNode &config) override;
};

class GameRawImageAssetType : public GameAssetType<Aoba::BaseImage> {
public:
  //! Loads the sprite
  GameRawImageAsset *create(const QString &id) const override;

protected:
  //! Loads the type configuration
  Aoba::RawImageAssetSet *configure(const GameConfigNode &node) override;

  //! Creates the asset resolver
  Aoba::ScriptAssetResolver *createAssetResolver() const override;
  
};

}