#include <QGraphicsRectItem>
#include <maker/game/GameTilemapAssetType.hpp>
#include <maker/game/GameTilesetAssetType.hpp>
#include <maker/map/ExitMapSceneLayer.hpp>
#include <maker/map/MapScene.hpp>
#include <maker/tileset/TileRenderHelper.hpp>
#include <maker/map/MapEditorContext.hpp>
#include <rpg/tileset/TilesetCollisionModule.hpp>

// -----------------------------------------------------------------------------
void ExitMapSceneLayer::renderOverlay(MapSceneLayerRenderInfo &render, QPainter &) {
  clearSceneItems();

  Aoba::Tilemap *tilemap = render.mapScene()->tilemap()->asset();
  Aoba::Tileset *tileset = render.mapScene()->tileset()->asset();
  Aoba::TilemapSwitch *sw = tilemap->tilemapSwitch(mSwitchId);

  for (auto *layerConfig : tilemap->assetSet()->layers()) {
    for (int y=render.tileRect().top(), toY=0; y<=render.tileRect().bottom(); y++, toY+=16) {
      for (int x=render.tileRect().left(), toX=0; x<=render.tileRect().right(); x++, toX+=16) {
        Aoba::TilesetLayer *tilesetLayer = tileset->layer(layerConfig->tilesetLayer());
        const Aoba::TilemapTile &ref = sw->layer(layerConfig)->generatedTile(x, y, true);
        Aoba::TilesetTile *tile = tilesetLayer->allTiles()[ref.tile.originalIndex];
        Rpg::TilesetTileCollision info = mModule->forTile(tile);

        if (!info.exit) {
          continue;
        }

        createExit(render, QPoint(x, y), render.renderRect().topLeft() + QPoint(toX, toY));
      }
    }
  }
}

// -----------------------------------------------------------------------------
void ExitMapSceneLayer::createExit(MapSceneLayerRenderInfo &render, const QPoint &tile, const QPoint &renderPosition) {
  (void)tile;

  QGraphicsRectItem *rect = new QGraphicsRectItem(QRect(
    renderPosition,
    QSize(16, 16)
  ));
  rect->setPen(QPen(QColor(255, 0, 0, 255)));
  rect->setZValue(1);

  addSceneItem(render, rect);
}