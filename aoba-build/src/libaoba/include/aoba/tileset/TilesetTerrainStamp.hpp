#pragma once

#include <QMap>
#include <QPoint>
#include <QVector>
#include <yaml-cpp/yaml.h>
#include "TilesetTileRef.hpp"

namespace Aoba {

class Tileset;

typedef QVector<TilesetTileRef> TilesetTerrainTileList;
typedef QMap<QString, TilesetTerrainTileList> TilesetTerrainTileRandomGroup;

class TilesetTerrainStamp {
public:
  enum Checkerboard {
    NO_CHECKERBOARD,
    CHECKERBOARD_HORIZONTAL,
    CHECKERBOARD_VERTICAL
  };

  //! Loads from YAML
  bool load(Tileset *tileset, const YAML::Node &);

  //! Saves to YAML
  bool save(YAML::Emitter &) const;

  //! Applies the terrain
  void apply(TilemapTerrainLayer*, TilemapTerrainBuildContext &context, const QPoint &pos) const;

  //! Returns the size
  inline const QSize &size() const { return mSize; }

  //! Returns the checkerboard mode
  inline const Checkerboard &checkerboard() const { return mCheckerboard; }

private:
  //! Returns a tile list
  TilesetTerrainTileList loadTileList(const YAML::Node &) const;

  //! Returns a tile list
  TilesetTileRef loadTileItem(const YAML::Node &) const;

  //! Seed
  int mSeed;

  //! Size of the stamp
  QSize mSize;

  //! Offset
  QPoint mOffset;

  //! Data
  QVector<TilesetTerrainTileRandomGroup> mTiles;

  //! Checkerboard mode
  Checkerboard mCheckerboard = NO_CHECKERBOARD;
};

}
