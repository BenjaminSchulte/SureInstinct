#include <aoba/log/Log.hpp>
#include <QUndoStack>
#include <maker/game/GameTilemapAssetType.hpp>
#include <maker/map/SetTileUndoCommand.hpp>
#include <maker/map/AbstractTileMapSceneLayer.hpp>
#include <maker/map/MapEditorContext.hpp>
#include <maker/map/MapScene.hpp>

// -----------------------------------------------------------------------------
const TileBrush &AbstractTileMapSceneLayer::brush(const MapScene *scene) const {
  return scene->context()->tileBrush();
}

// -----------------------------------------------------------------------------
TileBrushTile AbstractTileMapSceneLayer::getTile(const MapScene *scene, const QPoint &pos) const {
  const auto &tile = sceneLayer(scene)->overlayTile(pos.x(), pos.y(), true);
  
  return TileBrushTile(tile.tile.originalIndex, tile.tile.flipX, tile.tile.flipY);
}

// -----------------------------------------------------------------------------
void AbstractTileMapSceneLayer::setTiles(MapScene *scene, const PositionedTileList &changes) const {
  scene->undoStack()->push(new SetTileUndoCommand(scene, mLayer, mSwitchId, changes));
}

// -----------------------------------------------------------------------------
void AbstractTileMapSceneLayer::commitSetTile(MapScene *scene) {
  scene->undoStack()->push(new SetTileUndoCommand());
}

// -----------------------------------------------------------------------------
Aoba::TilemapLayer *AbstractTileMapSceneLayer::sceneLayer(const MapScene *scene) const {
  return scene->tilemap()->asset()->tilemapSwitch(mSwitchId)->layer(mLayer);
}