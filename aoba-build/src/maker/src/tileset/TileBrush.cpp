#include <aoba/tilemap/TilemapTile.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <maker/tileset/TileBrush.hpp>

// -----------------------------------------------------------------------------
TileBrushTile TileBrush::tile(int x, int y) const {
  if (mSize.width() == 0 || mSize.height() == 0) {
    return TileBrushTile();
  }

  if (x < 0) {
    x = mSize.width() - ((mSize.width() - x - 1) % mSize.width()) - 1;
  } else {
    x %= mSize.width();
  }
  if (y < 0) {
    y = mSize.height() - ((mSize.height() - y - 1) % mSize.height()) - 1;
  } else {
    y %= mSize.height();
  }

  if (mFlipX) {
    x = mSize.width() - 1 - x;
  }
  if (mFlipY) {
    y = mSize.height() - 1 - y;
  }

  return mTiles[x + y * mSize.width()].flipped(mFlipX, mFlipY);
}

// -----------------------------------------------------------------------------
Aoba::TilemapTile TileBrushTile::tilemapTile(Aoba::TilesetLayer *layer) const {
  Aoba::TilesetTileRef ref(layer->require(mTile, true));
  ref.flipX ^= mFlipX;
  ref.flipY ^= mFlipY;
  return Aoba::TilemapTile(ref);
}
