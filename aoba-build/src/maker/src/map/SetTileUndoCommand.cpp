#include <aoba/log/Log.hpp>
#include <QPoint>
#include <maker/game/GameTilemapAssetType.hpp>
#include <aoba/tilemap/Tilemap.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/tileset/TilesetTile.hpp>
#include <maker/map/MapScene.hpp>
#include <maker/map/SetTileUndoCommand.hpp>

// -----------------------------------------------------------------------------
SetTileUndoCommand::SetTileUndoCommand() {}

// -----------------------------------------------------------------------------
SetTileUndoCommand::SetTileUndoCommand(MapScene *scene, Aoba::TilemapAssetSetLayer *layer, const QString &switchId, const PositionedTileList &list)
  : mScene(scene)
  , mLayer(layer)
  , mSwitchId(switchId)
  , mFinal(false)
{
  for (const auto &item : list) {
    addTile(item.position, item.tile);
  }
}

// -----------------------------------------------------------------------------
SetTileUndoCommand::SetTileUndoCommand(MapScene *scene, Aoba::TilemapAssetSetLayer *layer, const QString &switchId, const QPoint &position, const TileBrushTile &newTile)
  : mScene(scene)
  , mLayer(layer)
  , mSwitchId(switchId)
  , mFinal(false)
{
  addTile(position, newTile);
}

// -----------------------------------------------------------------------------
Aoba::TilemapLayer *SetTileUndoCommand::getLayer() const {
  return mScene->tilemap()->asset()->tilemapSwitch(mSwitchId)->layer(mLayer);
}

// -----------------------------------------------------------------------------
void SetTileUndoCommand::addTile(const QPoint &position, const TileBrushTile &newTile) {
  Aoba::TilemapLayer *layer = getLayer();
  if (position.x() < 0 || position.y() < 0 || position.x() >= layer->width() || position.y() >= layer->height()) {
    return;
  }

  const Aoba::TilesetTileRef &tile = layer->overlayTile(position.x(), position.y(), true).tile;
  int originalIndex = tile.originalIndex;

  TileBrushTile oldTile(originalIndex, tile.flipX, tile.flipY);
  
  if (oldTile != newTile) {
    mChanges.push_back(SetTileUndoCommandPair(position, oldTile, newTile));
  }
}

// -----------------------------------------------------------------------------
bool SetTileUndoCommand::mergeWithCommand(const SetTileUndoCommand *other) {
  bool layerIsDifferent = other->mLayer != mLayer || other->mScene != mScene || other->mSwitchId != mSwitchId;
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
void SetTileUndoCommand::redo() {
  if (!mScene || !mLayer) {
    return;
  }

  Aoba::TilesetLayer *tilesetLayer = mScene->tilemap()->asset()->tileset()->layer(mLayer->tilesetLayer());
  Aoba::TilemapLayer *layer = getLayer();

  QRect updateRect;
  for (const auto &change : mChanges) {
    updateRect = updateRect.united(QRect(change.mPoint, QSize(1, 1)));
    layer->setTile(change.mPoint.x(), change.mPoint.y(), change.mNewTile.tilemapTile(tilesetLayer), true);
  }

  if (!updateRect.isEmpty()) {
    mScene->redrawTilemap(QRect(
      updateRect.left() - 1,
      updateRect.top() - 1,
      updateRect.width() + 2,
      updateRect.height() + 2
    ));
  }
}

// -----------------------------------------------------------------------------
void SetTileUndoCommand::undo() {
  if (!mScene || !mLayer) {
    return;
  }
  
  Aoba::TilesetLayer *tilesetLayer = mScene->tilemap()->asset()->tileset()->layer(mLayer->tilesetLayer());
  Aoba::TilemapLayer *layer = getLayer();

  QRect updateRect;
  for (const auto &change : mChanges) {
    updateRect = updateRect.united(QRect(change.mPoint, QSize(1, 1)));
    layer->setTile(change.mPoint.x(), change.mPoint.y(), change.mOldTile.tilemapTile(tilesetLayer), true);
  }

  if (!updateRect.isEmpty()) {
    mScene->redrawTilemap(updateRect);
  }
}