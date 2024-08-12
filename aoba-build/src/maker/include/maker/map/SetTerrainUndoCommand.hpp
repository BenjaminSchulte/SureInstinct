#pragma once

#include <QVector>
#include <QPoint>
#include "../common/AobaUndoCommand.hpp"
#include "../tileset/TileBrush.hpp"

namespace Aoba {
class Tilemap;
class TilemapTerrainLayer;
}

class MapScene;

struct SetTerrainUndoCommandPair {
  SetTerrainUndoCommandPair() {}
  SetTerrainUndoCommandPair(const QPoint &point, int old, int newTile)
    : mPoint(point), mOldTile(old), mNewTile(newTile)
  {}

  QPoint mPoint;
  int mOldTile;
  int mNewTile;
};

class SetTerrainUndoCommand : public AobaUndoCommandTemplate<SetTerrainUndoCommand> {
public:
  //! Empty constructor
  SetTerrainUndoCommand();

  //! Data constructor
  SetTerrainUndoCommand(MapScene *, const QString &, const QString &, const PositionedTileList &);

  //! Data constructor
  SetTerrainUndoCommand(MapScene *, const QString &, const QString &, const QPoint &, int);

  //! Returns the ID
  int id() const override { return (int)AobaUndoCommandType::SET_TERRAIN; }

  //! Tries to merge
  bool mergeWithCommand(const SetTerrainUndoCommand *other) override;

  //! Redo
  void redo() override;

  //! Undo
  void undo() override;

private:
  //! Returns the layer
  Aoba::TilemapTerrainLayer *getLayer() const;

  //! Adds a tile
  void addTile(const QPoint &position, int newTile);

  //! List of all changes
  QVector<SetTerrainUndoCommandPair> mChanges;

  //! The scene
  MapScene *mScene = nullptr;

  //! The tilemap
  QString mLayerId;

  //! The tilemap
  QString mSwitchId;

  //! Whether this list is final
  bool mFinal = true;
};
