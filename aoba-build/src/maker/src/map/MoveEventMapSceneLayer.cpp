#include <aoba/tilemap/Tilemap.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <maker/game/GameTilemapAssetType.hpp>
#include <maker/game/GameTilesetAssetType.hpp>
#include <maker/map/MoveEventMapSceneLayer.hpp>
#include <maker/map/MapScene.hpp>
#include <maker/tileset/TileRenderHelper.hpp>
#include <maker/map/MapEditorContext.hpp>
#include <rpg/tileset/TilesetCollisionModule.hpp>

// -----------------------------------------------------------------------------
void MoveEventMapSceneLayer::renderOverlay(MapSceneLayerRenderInfo &render, QPainter &) {
  clearSceneItems();
  
  for (int y=render.tileRect().top(), toY=0; y<=render.tileRect().bottom(); y++, toY+=16) {
    for (int x=render.tileRect().left(), toX=0; x<=render.tileRect().right(); x++, toX+=16) {
      Rpg::TilesetTileCollision self = collisionForTile(render, x, y);

      addSceneItem(render, TileRenderHelper::createMovementIcon(QPoint(toX, toY), self, false));
    }
  }
}