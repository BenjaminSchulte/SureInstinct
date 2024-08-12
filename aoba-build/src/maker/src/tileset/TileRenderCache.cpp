#include <QPainter>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/tileset/TilesetTile.hpp>
#include <aoba/palette/Palette.hpp>
#include <aoba/palette/PaletteGroup.hpp>
#include <aoba/palette/SnesPalette.hpp>
#include <aoba/image/PaletteImageAccessor.hpp>
#include <maker/tileset/TileRenderCache.hpp>

// -----------------------------------------------------------------------------
const TileRenderCacheLayer &TileRenderCache::get(Aoba::TilesetLayer *layer) {
  if (mCache.contains(layer)) {
    mUsageOrder.removeAll(layer);
    mUsageOrder.push_back(layer);
    return mCache[layer];
  }

  if (mUsageOrder.size() == mMaxRecords) {
    mCache.remove(mUsageOrder.front());
    mUsageOrder.pop_front();
  }

  mUsageOrder.push_back(layer);
  cacheLayer(layer);

  return mCache[layer];
}

// -----------------------------------------------------------------------------
void TileRenderCache::cacheLayer(Aoba::TilesetLayer *layer) {
  Aoba::Tileset *tileset = layer->tileset();
  int numBgTilesX = tileset->assetSet()->numBgTilesX();
  int numBgTilesY = tileset->assetSet()->numBgTilesY();
  int bgTileWidth = tileset->assetSet()->bgTileWidth();
  int bgTileHeight = tileset->assetSet()->bgTileHeight();

  Aoba::TilesetAssetSetLayer *layerConfig = layer->config();
  Aoba::TilesetTiles *tiles = tileset->tiles(layerConfig->tiles());
  TileRenderCacheLayer &cache = mCache[layer];

  int numColors = layerConfig->tiles()->paletteGroupSet()->paletteSet()->numSnesColors();

  for (Aoba::TilesetTile *layerTile : layer->allTiles()) {
    QPixmap frontTile(bgTileWidth * numBgTilesX, bgTileHeight * numBgTilesY);
    QPixmap backTile(bgTileWidth * numBgTilesX, bgTileHeight * numBgTilesY);
    frontTile.fill(QColor(0, 0, 0, 0));
    backTile.fill(QColor(0, 0, 0, 0));

    QPainter frontTilePainter(&frontTile);
    QPainter backTilePainter(&backTile);
    for (uint8_t bg=0; bg<layer->config()->numBackgrounds(); bg++) {
      for (int y=0, i=0; y<numBgTilesY; y++) {
        for (int x=0; x<numBgTilesX; x++, i++) {
          const Aoba::TilesetTilePart &part = layerTile->tileLayer(bg)->part(i);
          const Aoba::TilesetBgTile &bgTile = tiles->bgTiles()[part.bgTileIndex()];
          Aoba::SnesPaletteView palette = tiles->previewPalette()->subPalette(bgTile.palette, numColors, true);

          QImagePtr image((Aoba::PaletteImageAccessor(bgTile.data)).assignPalette(palette));
          if (part.priority()) {
            frontTilePainter.drawImage(QPoint(x * bgTileWidth, y * bgTileHeight), *image.get());
          } else {
            backTilePainter.drawImage(QPoint(x * bgTileWidth, y * bgTileHeight), *image.get());
          }
        }
      }
    }

    cache.frontTiles.push_back(frontTile);
    cache.backTiles.push_back(backTile);
  }
}