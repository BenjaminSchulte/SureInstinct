#pragma once

#include "../tileset/TilesetTileRef.hpp"

namespace Aoba {

struct TilemapTile {
  TilemapTile() {}
  TilemapTile(const TilesetTileRef &tile) : tile(tile) {}

  TilesetTileRef tile;
};

}