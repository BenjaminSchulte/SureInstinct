#include <QPixmap>
#include <maker/map/MapScreenshotHelper.hpp>
#include <aoba/level/Level.hpp>
#include <aoba/level/LevelTilemapComponent.hpp>
#include <aoba/tilemap/Tilemap.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <maker/game/GameTilemapAssetType.hpp>
#include <maker/game/GameLevelAssetType.hpp>
#include <maker/tileset/TileRenderHelper.hpp>

// -----------------------------------------------------------------------------
QImage MapScreenshotHelper::screenshot(Maker::GameLevelAsset *level) const {
  Maker::GameTilemapAsset *gameTilemap = level->tilemap();
  if (!gameTilemap) {
    return QImage();
  }

  Aoba::Tilemap *tilemap = gameTilemap->asset();

  QSize tileSize(tilemap->tileset()->assetSet()->tileSize());
  QRect tilesRect(0, 0, tilemap->width(), tilemap->height());
  QPixmap main(tilemap->width() * tileSize.width(), tilemap->height() * tileSize.height());
  QPixmap sub(tilemap->width() * tileSize.width(), tilemap->height() * tileSize.height());
  main.fill(QColor(0, 0, 0, 0));
  sub.fill(QColor(0, 0, 0, 0));

  for (const Aoba::LevelTilemapRenderOrder &layer : level->tilemapRenderOrder()) {
    MapScreenshotConfiguration config(layer.renderPriority, layer.renderNonPriority, layer.main, layer.sub);
    renderLayer(tilemap, layer.layer, "", tilesRect, main, sub, config);
  }

  QPainter mainPainter(&main);
  mainPainter.drawPixmap(QPoint(0, 0), sub);
  
  return main.toImage();
}

// -----------------------------------------------------------------------------
void MapScreenshotHelper::renderLayer(Aoba::Tilemap *tilemap, Aoba::TilemapAssetSetLayer *layer, const QString &switchId, const QRect &tiles, QPixmap &main, QPixmap &sub, const MapScreenshotConfiguration &config) const {
  Aoba::Tileset *tileset = tilemap->tileset();
  TileRenderHelper fgHelper(&main);
  TileRenderHelper bgHelper(&sub);

  Aoba::TilemapSwitch *sw = tilemap->tilemapSwitch(switchId);

  sw->regenerateTerrain();

  QSize tileSize(tilemap->tileset()->assetSet()->tileSize());
  Aoba::TilesetLayer *tilesetLayer = tileset->layer(layer->tilesetLayer());
  for (int y=tiles.top(), toY=0; y<=tiles.bottom(); y++, toY+=tileSize.width()) {
    for (int x=tiles.left(), toX=0; x<=tiles.right(); x++, toX+=tileSize.height()) {
      const Aoba::TilemapTile &ref = sw->layer(layer)->generatedTile(x, y, true);
      Aoba::TilesetTile *tile = tilesetLayer->allTiles()[ref.tile.originalIndex];

      if (config.main()) {
        fgHelper.renderTile(tilesetLayer, tile, toX, toY, config.front(), config.back(), ref.tile.flipX, ref.tile.flipY);
        if (!switchId.isEmpty() && ref.tile.originalIndex == 0) {
          fgHelper.renderSwitchInactive(toX, toY);
        }
      }
      if (config.sub()) {
        bgHelper.renderTile(tilesetLayer, tile, toX, toY, config.front(), config.back(), ref.tile.flipX, ref.tile.flipY);
        if (!switchId.isEmpty() && ref.tile.originalIndex == 0) {
          bgHelper.renderSwitchInactive(toX, toY);
        }
      }
    }
  }
}
