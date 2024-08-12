#include <aoba/log/Log.hpp>
#include <QPainter>
#include <aoba/image/PaletteImageAccessor.hpp>
#include <aoba/tileset/TilesetTile.hpp>
#include <maker/tileset/TilesetContext.hpp>
#include <aoba/tilemap/Tilemap.hpp>
#include <aoba/palette/PaletteGroup.hpp>
#include <aoba/palette/Palette.hpp>
#include <aoba/palette/SnesPalette.hpp>
#include <maker/game/GameTilesetAssetType.hpp>
#include <maker/map/MapSceneLayer.hpp>
#include <maker/map/MapSceneLayerList.hpp>

// -----------------------------------------------------------------------------
TilesetContext::TilesetContext(QObject *parent)
  : QObject(parent)
  , mTilesetReader(nullptr)
{
}

// -----------------------------------------------------------------------------
void TilesetContext::setTileset(Aoba::Tileset *set) {
  if (set == mTileset) {
    return;
  }
  
  mTileset = set;
  if (set == nullptr) {
    mTilesetLayer = nullptr;
    mTilesetReader.setLayer(nullptr);
  } else {
    mTilesetLayer = set->layer(set->assetSet()->layer().first());
    mTilesetReader.setLayer(mTilesetLayer);
  }

  emit tilesetChanged();
  emit tilesetLayerChanged();
}

// -----------------------------------------------------------------------------
void TilesetContext::setTilesetLayer(Aoba::TilesetLayer *set) {
  if (set == mTilesetLayer) {
    return;
  }
  
  mTilesetLayer = set;
  mTilesetReader.setLayer(mTilesetLayer);
  
  emit tilesetLayerChanged();
}

// -----------------------------------------------------------------------------
void TilesetContext::setTileBrush(const TileBrush &brush) {
  mTileBrush = brush;
  flipBrush(mBrushFlipX, mBrushFlipY);
  emit brushChanged();
}

// -----------------------------------------------------------------------------
void TilesetContext::setBrushFlip(bool x, bool y) {
  if (mBrushFlipX == x && mBrushFlipY == y) {
    return;
  }

  bool changeX = mBrushFlipX ^ x;
  bool changeY = mBrushFlipY ^ y;
  flipBrush(changeX, changeY);
  mBrushFlipX = x;
  mBrushFlipY = y;
}

// -----------------------------------------------------------------------------
void TilesetContext::flipBrush(bool x, bool y) {
  if (!x && !y) {
    return;
  }

  mTileBrush = mTileBrush.flipped(x, y);
}
