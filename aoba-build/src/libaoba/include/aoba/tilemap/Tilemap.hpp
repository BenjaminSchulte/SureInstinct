#pragma once

#include <QDir>
#include <QRect>
#include <QVector>
#include "TilemapTile.hpp"
#include "TilemapConfig.hpp"

namespace Aoba {
class Tileset;
class TilemapSwitch;
class TilemapTerrain;


class TilemapLayer {
public:
  //! Constructor
  TilemapLayer(TilemapSwitch *tilemap, TilemapAssetSetLayer *config)
    : mTilemapSwitch(tilemap)
    , mConfig(config)
  {}

  //! Returns the tilemap
  inline TilemapSwitch *tilemapSwitch() const { return mTilemapSwitch; }

  //! Returns the tilemap
  Tilemap *tilemap() const;

  //! Returns the tilemap config
  inline TilemapAssetSetLayer *config() const { return mConfig; }
  
  //! Returns a tile
  TilemapTile overlayTile(int x, int y, bool applyLayerMirror=false) const;
  
  //! Returns a tile
  TilemapTile terrainTile(int x, int y, bool applyLayerMirror=false) const;
  
  //! Returns a tile
  TilemapTile generatedTile(int x, int y, bool applyLayerMirror=false) const;

  //! Sets a tile
  void setTile(int x, int y, const TilemapTile &, bool applyLayerMirror=false);

  //! Sets the tiles
  void setTiles(const QVector<TilemapTile> &tiles, uint16_t width, uint16_t height);

  //! Requires the terrain tiles array to be valid
  void requireTerrainTileArray();

  //! Sets the terrain tiles
  void setTerrainTiles(const QVector<TilemapTile> &tiles);

  //! Sets a terrain tile
  inline void setTerrainTile(int index, const TilemapTile &tile) {
    mTerrainTiles[index] = tile;
  }

  //! Returns the width
  inline uint16_t width() const { return mWidth; }

  //! Returns the height
  inline uint16_t height() const { return mHeight; }

  //! Resizes the tilemap
  bool resize(int left, int top, int right, int bottom);

private:
  //! The tilemap
  TilemapSwitch *mTilemapSwitch;

  //! The tilemap config
  TilemapAssetSetLayer *mConfig;

  //! The tiles
  QVector<TilemapTile> mTiles;

  //! The tiles
  QVector<TilemapTile> mTerrainTiles;

  //! The current width
  uint16_t mWidth = 0;

  //! The current height
  uint16_t mHeight = 0;
};


class TilemapSwitch {
public:
  TilemapSwitch(Tilemap *parent) : mTilemap(parent) {}

  //! Deconstructor
  ~TilemapSwitch();

  //! Returns the tilemap
  inline Tilemap *tilemap() const { return mTilemap; }

  //! Returns a layer
  TilemapLayer *layer(TilemapAssetSetLayer *);

  //! Returns the terrain
  inline TilemapTerrain *terrain() const { return mTerrain; }

  //! Returns the terrain
  TilemapTerrain *requireTerrain();

  //! Invalidates the tilemap
  void invalidateTerrain(const QRect &rect);

  //! Invalidates the tilemap
  void regenerateTerrain();

  //! Resizes the tilemap
  bool resize(int left, int top, int right, int bottom);

private:
  //! The tilemap
  Tilemap *mTilemap;

  //! All layers
  QMap<QString, TilemapLayer*> mLayers;

  //! The terrain
  TilemapTerrain *mTerrain = nullptr;

  //! Whether the terrain is valid
  QRect mInvalidTerrain;
};


class Tilemap : public Asset<TilemapAssetSet> {
public:
  //! Constructor
  Tilemap(TilemapAssetSet *set, const QString &id, const QDir &path);

  //! Deconstructor
  ~Tilemap();

  //! Returns the tilemap symbol
  QString symbolName() const;

  //! Saves the tilemap
  bool save() override;

  //! Loads the asset
  bool reload() override;

  //! Builds the asset
  bool build() override;
  
  //! Returns the FMA object file names
  QStringList getFmaObjectFiles() const override;

  //! Returns the necessary FMA code
  QString getFmaCode() const override;

  //! Returns the path
  inline const QDir &path() const { return mPath; }

  //! Sets the size
  void setSize(uint16_t w, uint16_t h);

  //! Sets the tileset
  inline void setTileset(Tileset *tileset) {
    mTileset = tileset;
  }

  //! Returns the tileset
  inline Tileset *tileset() const { return mTileset; }

  //! Returns the width
  inline uint16_t width() const { return mWidth; }

  //! Returns the height
  inline uint16_t height() const { return mHeight; }

  //! Resizes the tilemap
  bool resize(int left, int top, int right, int bottom);

  //! Returns the map data
  inline const TilemapSwitch &mapData() const { return mMainSwitch; }

  //! Returns the map data
  inline TilemapSwitch &mapData() { return mMainSwitch; }

  //! Returns the tilemap switch
  TilemapSwitch *requireTilemapSwitch(const QString &id);

  //! Returns the tilemap switch
  TilemapSwitch *tilemapSwitch(const QString &id) { return id.isEmpty() ? &mMainSwitch : mCustomSwitchList[id]; }

  //! Returns if a switch exists
  inline bool hasSwitch(const QString &id) const { return mCustomSwitchList.contains(id) || id.isEmpty(); }

  //! Returns all switches
  inline const QMap<QString, TilemapSwitch*> &tilemapSwitches() const { return mCustomSwitchList; }

protected:
  //! The containing path
  QDir mPath;

  //! The tileset
  Tileset *mTileset;

  //! The width
  uint16_t mWidth = 0;

  //! The height
  uint16_t mHeight = 0;

  //! The main "switch" which actually is the level
  TilemapSwitch mMainSwitch;

  //! Custom switch list
  QMap<QString, TilemapSwitch*> mCustomSwitchList;
};

class CustomTilemap : public Tilemap {
public:
  //! Constructor
  CustomTilemap(TilemapAssetSet *set, const QString &id, Tileset *tileset, uint16_t width, uint16_t height);

  //! Loads the asset
  bool reload() override;
};

}
