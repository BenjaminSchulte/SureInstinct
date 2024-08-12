#pragma once

#include <aoba/raw/RawAsset.hpp>
#include "GameAssetType.hpp"

namespace Maker {

class GameRawAsset : public GameAsset<Aoba::RawAsset> {
public:
  //! Constructor
  GameRawAsset(const GameAssetType<Aoba::RawAsset> *type, GameProject *project, const QString &id)
    : GameAsset<Aoba::RawAsset>(type, project, id)
  {}

  //! Creates the asset
  Aoba::RawAsset *configure(const GameConfigNode &config) override;
};

class GameRawAssetType : public GameAssetType<Aoba::RawAsset> {
public:
  //! Loads the palette
  GameRawAsset *create(const QString &id) const override;

protected:
  //! Loads the type configuration
  Aoba::RawAssetSet *configure(const GameConfigNode &node) override;

  //! Creates the asset resolver
  Aoba::ScriptAssetResolver *createAssetResolver() const override;
  
};

}