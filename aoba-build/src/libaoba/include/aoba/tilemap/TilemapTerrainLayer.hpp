#pragma once

#include <QString>
#include <QVector>
#include <QSize>

namespace Aoba {

class TilemapTerrain;
struct TilemapTerrainBuildContext;

class TilemapTerrainLayer {
public:
  //! Constructor
  TilemapTerrainLayer(TilemapTerrain *terrain, const QString &id)
    : mTerrain(terrain), mId(id) {}

  //! Deconstructor
  ~TilemapTerrainLayer();

  //! Resizes the tilemap
  bool resize(int left, int top, int right, int bottom);

  //! Builds the layer
  void build(TilemapTerrainBuildContext &context);

  //! Returns all tiles
  inline const int *tiles() const { return mTiles; }

  //! Returns all tiles
  inline int *tiles() { return mTiles; }

  //! Returns a specific tile
  int tile(int x, int y, bool respectMirror=false) const;

  //! Returns the terrain
  inline TilemapTerrain *terrain() const { return mTerrain; }

  //! Returns the ID
  inline const QString &id() const { return mId; }

  //! Returns the width
  inline int width() const { return mSize.width(); }

  //! Returns the height
  inline int height() const { return mSize.height(); }

  //! Sets a tile
  void setTile(int x, int y, int tile, bool respectMirror=false);

private:
  //! The terrain
  TilemapTerrain *mTerrain;

  //! The ID
  QString mId;

  //! The current size
  QSize mSize = QSize(0, 0);

  //! List of all terrain IDs
  int *mTiles = nullptr;
};

}