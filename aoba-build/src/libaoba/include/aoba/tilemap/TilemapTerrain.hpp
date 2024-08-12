#pragma once

#include <QMap>
#include <QRect>
#include <QVector>
#include "../utils/FlagMap2d.hpp"
#include "TilemapTile.hpp"

namespace Aoba {

class TilesetLayer;
class TilesetTerrain;
class TilesetTerrainLayer;
class Tilemap;
class TilemapSwitch;
class TilemapLayer;
class TilemapTerrainLayer;



struct TilemapTerrainBuildContextLayer {
  QVector<TilemapTile> tiles;
  TilesetLayer *tileset = nullptr;
  TilemapLayer *target = nullptr;
};

struct TilemapTerrianBuildContextTerrainLayer {
  QVector<bool> stopped;
  FlagMap2d tags;
  TilesetTerrainLayer *terrainLayer = nullptr;
};

struct TilemapTerrainBuildContext {
  QMap<QString, TilemapTerrainBuildContextLayer*> layers;
  QMap<TilemapTerrainLayer*, TilemapTerrianBuildContextTerrainLayer> terrainLayers;
  QVector<TilemapTerrainBuildContextLayer*> layerPointers;

  TilesetTerrain *tilesetTerrain;
  TilemapTerrainLayer *currentLayer;
  QRect updateRect;
};

class TilemapTerrain {
public:
  //! Constructor
  TilemapTerrain(TilemapSwitch *tilemap) : mTilemapSwitch(tilemap) {}

  //! Deconstructor
  ~TilemapTerrain();

  //! Removes all layers
  void clear();

  //! Resizes the tilemap
  bool resize(int left, int top, int right, int bottom);
  
  //! Builds the terrain
  void build(const QRect &updateRect);

  //! Returns the tilemap
  inline TilemapSwitch *tilemapSwitch() const { return mTilemapSwitch; }

  //! Returns the tilemap
  Tilemap *tilemap() const;

  //! Returns a layer
  TilemapTerrainLayer *getOrCreateLayer(const QString &id);

  //! Returns a layer
  TilemapTerrainLayer *layer(const QString &id) const { return mLayer[id]; }

  //! Returns the layers
  inline const QMap<QString, TilemapTerrainLayer*> &layers() const { return mLayer; }

private:
  //! The tilemap
  TilemapSwitch *mTilemapSwitch;

  //! List of all layers
  QMap<QString, TilemapTerrainLayer*> mLayer;
};

}