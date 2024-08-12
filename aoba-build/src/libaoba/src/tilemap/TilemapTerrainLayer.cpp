#include <QDebug>
#include <aoba/log/Log.hpp>
#include <aoba/tileset/TilesetTerrain.hpp>
#include <aoba/tileset/TilesetTerrainLayer.hpp>
#include <aoba/tileset/TilesetTerrainType.hpp>
#include <aoba/tileset/TilesetTerrainRuleGroup.hpp>
#include <aoba/tilemap/Tilemap.hpp>
#include <aoba/tilemap/TilemapTerrain.hpp>
#include <aoba/tilemap/TilemapTerrainLayer.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
TilemapTerrainLayer::~TilemapTerrainLayer() {
  delete mTiles;
}

// -----------------------------------------------------------------------------
void TilemapTerrainLayer::build(TilemapTerrainBuildContext &context) {
  TilemapTerrianBuildContextTerrainLayer &layerContext = context.terrainLayers[this];

  for (int index=0, y=0; y<mSize.height(); y++) {
    for (int x=0; x<mSize.width(); x++, index++) {
      int terrainId = mTiles[index];
      if (terrainId == -1) {
        continue;
      }

      TilesetTerrainType *type = context.tilesetTerrain->type(terrainId);
      if (!type) {
        continue;
      }

      layerContext.tags.setMulti(QPoint(x, y), type->tags());
    }
  }

  for (TilesetTerrainRuleGroup *group : context.tilesetTerrain->groups()) {
    for (int y=context.updateRect.top(); y<=context.updateRect.bottom(); y++) {
      for (int x=context.updateRect.left(); x<=context.updateRect.right(); x++) {
        group->apply(this, context, QPoint(x, y));
      }
    }
  }
}

// -----------------------------------------------------------------------------
bool TilemapTerrainLayer::resize(int left, int top, int right, int bottom) {
  QSize newSize(mSize.width() + left + right, mSize.height() + top + bottom);
  if (newSize.isEmpty()) {
    return false;
  }

  int *tiles = new int[newSize.width() * newSize.height()];
  for (int i=0, y=0; y<newSize.height(); y++) {
    int fromY = y - top;
    for (int x=0; x<newSize.width(); x++, i++) {
      int fromX = x - left;

      if (fromX < 0 || fromY < 0 || fromX >= mSize.width() || fromY >= mSize.height()) {
        tiles[i] = -1;
      } else {
        tiles[i] = mTiles[fromX + fromY * mSize.width()];
      }
    }
  }

  delete mTiles;
  mTiles = tiles;
  mSize = newSize;

  return true;
}

// -----------------------------------------------------------------------------
int TilemapTerrainLayer::tile(int x, int y, bool respectMirror) const {
  if (respectMirror) {
    TilesetTerrainLayer *layer = mTerrain->tilemap()->tileset()->terrain()->layer(mId);

    x = (layer->editorMirrorX()) ? (width() - x - 1) : x;
    y = (layer->editorMirrorY()) ? (height() - y - 1) : y;
  }

  return mTiles[x + y * mSize.width()];
}

// -----------------------------------------------------------------------------
void TilemapTerrainLayer::setTile(int x, int y, int tile, bool respectMirror) {
  if (respectMirror) {
    TilesetTerrainLayer *layer = mTerrain->tilemap()->tileset()->terrain()->layer(mId);

    x = (layer->editorMirrorX()) ? (width() - x - 1) : x;
    y = (layer->editorMirrorY()) ? (height() - y - 1) : y;
  }

  mTiles[x + y * width()] = tile;
  mTerrain->tilemapSwitch()->invalidateTerrain(QRect(x, y, 1, 1));
}
