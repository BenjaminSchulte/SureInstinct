#include <aoba/log/Log.hpp>
#include <QUndoStack>
#include <aoba/tilemap/TilemapTerrainLayer.hpp>
#include <aoba/tilemap/TilemapTerrain.hpp>
#include <maker/game/GameTilemapAssetType.hpp>
#include <maker/map/SetTerrainUndoCommand.hpp>
#include <maker/map/TerrainMapSceneLayer.hpp>
#include <maker/map/MapEditorContext.hpp>
#include <maker/map/MapScene.hpp>

// -----------------------------------------------------------------------------
const TileBrush &TerrainMapSceneLayer::brush(const MapScene *scene) const {
  return scene->context()->terrainBrush();
}

// -----------------------------------------------------------------------------
TileBrushTile TerrainMapSceneLayer::getTile(const MapScene *scene, const QPoint &pos) const {
  return TileBrushTile(scene->tilemap()->asset()->tilemapSwitch(mSwitchId)->terrain()->layer(mLayerId)->tile(pos.x(), pos.y(), true));
}

// -----------------------------------------------------------------------------
void TerrainMapSceneLayer::setTiles(MapScene *scene, const PositionedTileList &changes) const {
  scene->undoStack()->push(new SetTerrainUndoCommand(scene, mLayerId, mSwitchId, changes));
}

// -----------------------------------------------------------------------------
void TerrainMapSceneLayer::commitSetTile(MapScene *scene) {
  scene->undoStack()->push(new SetTerrainUndoCommand());
}
