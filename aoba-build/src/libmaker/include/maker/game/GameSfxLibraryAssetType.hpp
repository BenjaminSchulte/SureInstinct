#pragma once

#include <aoba/sfx/SfxLibrary.hpp>
#include "GameAssetType.hpp"

namespace Maker {

class GameSfxLibraryAsset : public GameAsset<Aoba::SfxLibrary> {
public:
  //! Constructor
  GameSfxLibraryAsset(const GameAssetType<Aoba::SfxLibrary> *type, GameProject *project, const QString &id)
    : GameAsset<Aoba::SfxLibrary>(type, project, id)
  {}

  //! Creates the asset
  Aoba::SfxLibrary *configure(const GameConfigNode &config) override;
};

class GameSfxLibraryAssetType : public GameAssetType<Aoba::SfxLibrary> {
public:
  //! Loads the palette
  GameSfxLibraryAsset *create(const QString &id) const override;

protected:
  //! Loads the type configuration
  Aoba::SfxLibraryAssetSet *configure(const GameConfigNode &node) override;

  //! Creates the asset resolver
  Aoba::ScriptAssetResolver *createAssetResolver() const override;
  
};

}