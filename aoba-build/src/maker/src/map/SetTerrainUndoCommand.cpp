#include <aoba/log/Log.hpp>
#include <QPoint>
#include <aoba/tilemap/Tilemap.hpp>
#include <aoba/tilemap/TilemapTerrain.hpp>
#include <aoba/tilemap/TilemapTerrainLayer.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/tileset/TilesetTile.hpp>
#include <maker/game/GameTilemapAssetType.hpp>
#include <maker/map/MapScene.hpp>
#include <maker/map/SetTerrainUndoCommand.hpp>

// -----------------------------------------------------------------------------
SetTerrainUndoCommand::SetTerrainUndoCommand() {}

// -----------------------------------------------------------------------------
SetTerrainUndoCommand::SetTerrainUndoCommand(MapScene *scene, const QString &layerId, const QString &switchId, const PositionedTileList &list)
  : mScene(scene)
  , mLayerId(layerId)
  , mSwitchId(switchId)
  , mFinal(false)
{
  for (const auto &item : list) {
    addTile(item.position, item.tile.tile());
  }
}

// -----------------------------------------------------------------------------
SetTerrainUndoCommand::SetTerrainUndoCommand(MapScene*scene, const QString &layerId, const QString &switchId, const QPoint &position, int newTile)
  : mScene(scene)
  , mLayerId(layerId)
  , mSwitchId(switchId)
  , mFinal(false)
{
  addTile(position, newTile);
}

// -----------------------------------------------------------------------------
Aoba::TilemapTerrainLayer *SetTerrainUndoCommand::getLayer() const {
  return mScene->tilemap()->asset()->tilemapSwitch(mSwitchId)->terrain()->layer(mLayerId);
}

// -----------------------------------------------------------------------------
void SetTerrainUndoCommand::addTile(const QPoint &position, int newTile) {
  Aoba::TilemapTerrainLayer *layer = getLayer();
  if (position.x() < 0 || position.y() < 0 || position.x() >= layer->width() || position.y() >= layer->height()) {
    return;
  }

  int oldTile = layer->tile(position.x(), position.y(), true);
  if (oldTile != newTile) {
    mChanges.push_back(SetTerrainUndoCommandPair(position, oldTile, newTile));
  }
}

// -----------------------------------------------------------------------------
bool SetTerrainUndoCommand::mergeWithCommand(const SetTerrainUndoCommand *other) {
  bool layerIsDifferent = other->mLayerId != mLayerId || other->mScene != mScene || other->mSwitchId != mSwitchId;
  if (mFinal || (layerIsDifferent && !other->mFinal)) {
    return false;
  }

  for (const auto &change : other->mChanges) {
    bool found = false;

    for (auto &ours : mChanges) {
      if (ours.mPoint != change.mPoint) {
        continue;
      }

      found = true;
      ours.mNewTile = change.mNewTile;
      break;
    }

    if (!found) {
      mChanges.push_back(change);
    }
  }

  mFinal = other->mFinal;
  return true;
}

// -----------------------------------------------------------------------------
void SetTerrainUndoCommand::redo() {
  if (!mScene || mLayerId.isEmpty()) {
    return;
  }

  Aoba::TilemapTerrainLayer *layer = getLayer();

  QRect updateRect;
  for (const auto &change : mChanges) {
    updateRect = updateRect.united(QRect(change.mPoint, QSize(1, 1)));
    layer->setTile(change.mPoint.x(), change.mPoint.y(), change.mNewTile, true);
  }
  
  layer->terrain()->tilemapSwitch()->invalidateTerrain(updateRect);

  if (!updateRect.isEmpty()) {
    mScene->redrawTilemap(QRect(
      updateRect.left() - 3,
      updateRect.top() - 3,
      updateRect.width() + 6,
      updateRect.height() + 6
    ));
  }
}

// -----------------------------------------------------------------------------
void SetTerrainUndoCommand::undo() {
  if (!mScene || mLayerId.isEmpty()) {
    return;
  }

  Aoba::TilemapTerrainLayer *layer = getLayer();

  QRect updateRect;
  for (const auto &change : mChanges) {
    updateRect = updateRect.united(QRect(change.mPoint, QSize(1, 1)));
    layer->setTile(change.mPoint.x(), change.mPoint.y(), change.mOldTile, true);
  }
  
  layer->terrain()->tilemapSwitch()->invalidateTerrain(updateRect);

  if (!updateRect.isEmpty()) {
    mScene->redrawTilemap(updateRect);
  }
}