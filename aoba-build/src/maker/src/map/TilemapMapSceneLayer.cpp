#include <aoba/tilemap/Tilemap.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <maker/game/GameTilemapAssetType.hpp>
#include <maker/game/GameTilesetAssetType.hpp>
#include <maker/map/TilemapMapSceneLayer.hpp>
#include <maker/map/MapScene.hpp>
#include <maker/tileset/TileRenderHelper.hpp>
#include <maker/map/MapEditorContext.hpp>

// -----------------------------------------------------------------------------
void TilemapMapSceneLayer::renderScreen(MapSceneLayerRenderInfo &render) {
  Aoba::Tilemap *tilemap = render.mapScene()->tilemap()->asset();
  MapScreenshotHelper screenshot;
  screenshot.renderLayer(tilemap, mLayer, mSwitchId, render.tileRect(), render.mainPixmap(), render.subPixmap(), mConfiguration);
}