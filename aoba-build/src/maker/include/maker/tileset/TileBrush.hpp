#pragma once

#include <QSize>
#include <QVector>
#include <QPoint>
#include <QMap>

namespace Aoba {
class TilesetLayer;
struct TilemapTile;
}

struct TileBrushTile {
  TileBrushTile() : mTile(0) {}
  TileBrushTile(int tile, bool flipX=false, bool flipY=false) : mTile(tile), mFlipX(flipX), mFlipY(flipY) {}

  inline int tile() const { return mTile; }
  inline bool flipX() const { return mFlipX; }
  inline bool flipY() const { return mFlipY; }

  TileBrushTile flipped(bool x, bool y) const {
    return TileBrushTile(mTile, mFlipX ^ x, mFlipY ^ y);
  }

  bool operator==(const TileBrushTile &other) const {
    return mTile==other.mTile && mFlipX==other.mFlipX && mFlipY==other.mFlipY;
  }
  bool operator!=(const TileBrushTile &other) const {
    return mTile!=other.mTile || mFlipX!=other.mFlipX || mFlipY!=other.mFlipY;
  }

  Aoba::TilemapTile tilemapTile(Aoba::TilesetLayer *) const;

private:
  int mTile;
  bool mFlipX = false;
  bool mFlipY = false;
};

struct PositionedTileBrushTile {
  PositionedTileBrushTile() {}
  PositionedTileBrushTile(const QPoint &pos, const TileBrushTile &tile)
    : position(pos), tile(tile)
  {}

  QPoint position;
  TileBrushTile tile;
};

typedef QVector<PositionedTileBrushTile> PositionedTileList;

struct TileBrush {
  TileBrush() : mSize(0, 0) {}
  TileBrush(const QVector<TileBrushTile> &tiles, const QSize &size, bool flipX=false, bool flipY=false)
    : mTiles(tiles), mSize(size), mFlipX(flipX), mFlipY(flipY) {}

  inline const QVector<TileBrushTile> &tiles() const { return mTiles; }
  inline const QSize &size() const { return mSize; }

  TileBrush flipped(bool flipX, bool flipY) {
    return TileBrush(mTiles, mSize, mFlipX ^ flipX, mFlipY ^ flipY);
  }

  inline TileBrushTile tile(const QPoint &pos) const { return tile(pos.x(), pos.y()); }
  TileBrushTile tile(int x, int y) const;

private:
  QVector<TileBrushTile> mTiles;
  QSize mSize;
  bool mFlipX;
  bool mFlipY;
};