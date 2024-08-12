#pragma once

#include <QVector>
#include <QPoint>
#include "../common/AobaUndoCommand.hpp"
#include "../tileset/TileBrush.hpp"

namespace Aoba {
class Tilemap;
class TilemapLayer;
}

class MapScene;

struct SetTileUndoCommandPair {
  SetTileUndoCommandPair() {}
  SetTileUndoCommandPair(const QPoint &point, const TileBrushTile &old, const TileBrushTile &newTile)
    : mPoint(point), mOldTile(old), mNewTile(newTile)
  {}

  QPoint mPoint;
  TileBrushTile mOldTile;
  TileBrushTile mNewTile;
};

class SetTileUndoCommand : public AobaUndoCommandTemplate<SetTileUndoCommand> {
public:
  //! Empty constructor
  SetTileUndoCommand();

  //! Data constructor
  SetTileUndoCommand(MapScene *, Aoba::TilemapAssetSetLayer*, const QString &switchId, const PositionedTileList &);

  //! Data constructor
  SetTileUndoCommand(MapScene *, Aoba::TilemapAssetSetLayer*, const QString &switchId, const QPoint &, const TileBrushTile &);

  //! Returns the ID
  int id() const override { return (int)AobaUndoCommandType::SET_TILE; }

  //! Tries to merge
  bool mergeWithCommand(const SetTileUndoCommand *other) override;

  //! Redo
  void redo() override;

  //! Undo
  void undo() override;

private:
  //! Converts the layer
  Aoba::TilemapLayer *getLayer() const;

  //! Adds a tile
  void addTile(const QPoint &position, const TileBrushTile &newTile);

  //! List of all changes
  QVector<SetTileUndoCommandPair> mChanges;

  //! The scene
  MapScene *mScene = nullptr;

  //! The tilemap
  Aoba::TilemapAssetSetLayer *mLayer = nullptr;

  //! The switch ID
  QString mSwitchId;

  //! Whether this list is final
  bool mFinal = true;
};
