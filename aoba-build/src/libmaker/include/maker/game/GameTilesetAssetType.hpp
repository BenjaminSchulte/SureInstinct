#pragma once

#include <aoba/tileset/Tileset.hpp>
#include "GameAsset.hpp"
#include "GameAssetType.hpp"

namespace Maker {

class GameTilesetAsset : public GameAsset<Aoba::Tileset> {
public:
  //! Constructor
  GameTilesetAsset(const GameAssetType<Aoba::Tileset> *type, GameProject *project, const QString &id)
    : GameAsset<Aoba::Tileset>(type, project, id)
  {}

  //! Creates the asset
  Aoba::Tileset *configure(const GameConfigNode &config) override;
};

class GameTilesetAssetType : public GameAssetType<Aoba::Tileset> {
public:
  //! Deconstructor
  ~GameTilesetAssetType();

  //! Loads the palette
  GameTilesetAsset *create(const QString &id) const override;

protected:
  //! Loads the type configuration
  Aoba::TilesetAssetSet *configure(const GameConfigNode &node) override;

  //! Adds the collision module
  bool addCollisionModule(Aoba::TilesetAssetSet *set, const GameConfigNode &node);

  //! Adds the collision module
  bool addTileFlagGroupModule(Aoba::TilesetAssetSet *set, const GameConfigNode &node);

  //! Adds the collision module
  bool addTileBitplane2dModule(Aoba::TilesetAssetSet *set, const GameConfigNode &node);

  //! Adds the collision module
  bool addTileListModule(Aoba::TilesetAssetSet *set, const GameConfigNode &node);

  //! Adds the collision module
  bool addTilesetAnimationModule(Aoba::TilesetAssetSet *set, const GameConfigNode &node);

  //! List of all tiles
  QMap<QString, Aoba::TilesetAssetSetTiles*> mTiles; 

  //! List of all layers
  QMap<QString, Aoba::TilesetAssetSetLayer*> mLayers; 

  //! List of all modules
  QVector<Aoba::TilesetAssetSetModule*> mModules;
};

}