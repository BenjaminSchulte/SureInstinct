#include <aoba/tilemap/Tilemap.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <maker/game/GameTilemapAssetType.hpp>
#include <maker/game/GameTilesetAssetType.hpp>
#include <maker/map/CollisionMapSceneLayer.hpp>
#include <maker/map/MapScene.hpp>
#include <maker/tileset/TileRenderHelper.hpp>
#include <maker/map/MapEditorContext.hpp>
#include <rpg/tileset/TilesetCollisionModule.hpp>

// -----------------------------------------------------------------------------
Rpg::TilesetTileCollision CollisionMapSceneLayer::collisionForTile(MapSceneLayerRenderInfo &render, int x, int y) const {
  Aoba::Tilemap *tilemap = render.mapScene()->tilemap()->asset();
  Aoba::Tileset *tileset = render.mapScene()->tileset()->asset();
  Aoba::TilesetLayer *tilesetLayer = tileset->layer(mLayer->tilesetLayer());

  if (x < 0 || y < 0 || x >= tilemap->width() || y >= tilemap->height()) {
    return Rpg::TilesetTileCollision();
  }

  const Aoba::TilemapTile &ref = sceneLayer(render.mapScene())->generatedTile(x, y, true);
  Aoba::TilesetTile *tile = tilesetLayer->allTiles()[ref.tile.originalIndex];
  Rpg::TilesetTileCollision col = mModule->forTile(tile);

  return col.flipped(ref.tile.flipX, ref.tile.flipY);
}

// -----------------------------------------------------------------------------
void CollisionMapSceneLayer::renderScreen(MapSceneLayerRenderInfo &render) {
  TileRenderHelper overlayHelper(&render.overlayPixmap());

  for (int y=render.tileRect().top(), toY=0; y<=render.tileRect().bottom(); y++, toY+=16) {
    for (int x=render.tileRect().left(), toX=0; x<=render.tileRect().right(); x++, toX+=16) {
      Rpg::TilesetTileCollision top = collisionForTile(render, x, y-1);
      Rpg::TilesetTileCollision btm = collisionForTile(render, x, y+1);
      Rpg::TilesetTileCollision left = collisionForTile(render, x-1, y);
      Rpg::TilesetTileCollision right = collisionForTile(render, x+1, y);
      Rpg::TilesetTileCollision self = collisionForTile(render, x, y);

      overlayHelper.renderCollision(toX, toY, self, top, right, btm, left);
    }
  }
}