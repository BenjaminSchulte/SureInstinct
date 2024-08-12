#pragma once

namespace Aoba {
class Tilemap;
class TilesetTerrainLayer;
class TilemapTerrainLayer;
class TilemapAssetSetLayer;

class TilemapLoader {
public:
  //! Constructor
  TilemapLoader(Tilemap *tilemap) : mTilemap(tilemap) {}

  //! Loads the tilemap
  bool load();

private:
  //! Reads the header
  bool readHeader();

  //! Reads switch data
  bool readTileAndTerrain(const QString &switchId);

  //! Reads a layer
  bool readLayer(const QString &prefix, TilemapSwitch *target, TilemapAssetSetLayer *);

  //! Reads a layer
  bool readTerrainLayer(const QString &prefix, TilemapSwitch *target, TilesetTerrainLayer *);

  // The tilemap
  Tilemap *mTilemap;

  //! List of switches
  QStringList mSwitchList;
};

}