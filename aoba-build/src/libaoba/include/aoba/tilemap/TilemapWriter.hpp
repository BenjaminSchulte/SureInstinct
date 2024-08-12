#pragma once

namespace Aoba {
class Tilemap;
class TilemapSwitch;
class TilemapTerrainLayer;
class TilemapAssetSetLayer;

class TilemapWriter {
public:
  //! Constructor
  TilemapWriter(Tilemap *tilemap) : mTilemap(tilemap) {}

  //! Loads the tilemap
  bool save();

private:
  //! writes the header
  bool writeHeader();

  //! Writes a switch
  bool writeSwitch(const QString &id);

  //! writes a layer
  bool writeLayer(const QString &prefix, TilemapSwitch *target, TilemapAssetSetLayer *);

  //! Writes a terrain
  bool writeTerrainLayer(const QString &prefix, TilemapSwitch *target, TilemapTerrainLayer *);

  // The tilemap
  Tilemap *mTilemap;
};

}