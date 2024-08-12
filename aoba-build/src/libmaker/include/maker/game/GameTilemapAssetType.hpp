#pragma once

#include <aoba/tilemap/Tilemap.hpp>
#include "GameAsset.hpp"
#include "GameAssetType.hpp"

namespace Aoba {
class Tileset;
}

namespace Maker {
class GameTilesetAsset;

class GameTilemapAsset : public GameAsset<Aoba::Tilemap> {
public:
  //! Constructor
  GameTilemapAsset(const GameAssetType<Aoba::Tilemap> *type, GameProject *project, const QString &id)
    : GameAsset<Aoba::Tilemap>(type, project, id)
  {}

  //! Creates the asset
  Aoba::Tilemap *createNew(Aoba::Tileset *tileset, const QSize &size);

  //! Creates the asset
  Aoba::Tilemap *configure(const GameConfigNode &config) override;

  //! Returns the tileset
  GameTilesetAsset *tileset() const;
};

class GameTilemapAssetType : public GameAssetType<Aoba::Tilemap> {
public:
  //! Deconstructor
  ~GameTilemapAssetType();

  //! Loads the palette
  GameTilemapAsset *create(const QString &id) const override;

protected:
  //! Loads the type configuration
  Aoba::TilemapAssetSet *configure(const GameConfigNode &node) override;

  //! List of all layers
  QVector<Aoba::TilemapAssetSetLayer*> mLayers;
};

}