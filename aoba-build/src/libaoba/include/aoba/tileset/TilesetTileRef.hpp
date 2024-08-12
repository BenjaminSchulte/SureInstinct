#pragma once

#include <QMap>

namespace Aoba {

struct TilesetTileRef {
  TilesetTileRef() {}

  TilesetTileRef(uint32_t index, uint32_t originalIndex, bool flipX=false, bool flipY=false)
    : index(index)
    , originalIndex(originalIndex)
    , flipX(flipX)
    , flipY(flipY)
  {}

  TilesetTileRef withFlip(bool x, bool y) const {
    return TilesetTileRef(index, originalIndex, x, y);
  }

  TilesetTileRef withOriginalIndex(int newOriginalIndex) const {
    return TilesetTileRef(index, newOriginalIndex, flipX, flipY);
  }

  int32_t index = -1;
  int32_t originalIndex = -1;
  bool flipX;
  bool flipY;
};

}