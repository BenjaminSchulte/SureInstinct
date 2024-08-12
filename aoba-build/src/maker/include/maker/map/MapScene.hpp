#pragma once

#include <QGraphicsScene>
#include <QPixmap>
#include <QVector>
#include "../tileset/TileBrush.hpp"

namespace Aoba {
class TilemapAssetSetLayer;
}

namespace Maker {
class GameLevelAsset;
class GameTilemapAsset;
class GameTilesetAsset;
}

class QUndoStack;
struct TileBrush;
struct TileBrushTile;
class MapEditorContext;
class MapSceneLayer;
class MapSceneLayerList;
class MapSceneTool;
class AobaGrid;

struct MapSceneHoverInfo {
  MapSceneHoverInfo() {}
  MapSceneHoverInfo(const QPoint &position) : mValid(true), mPosition(position) {}

  inline bool valid() const { return mValid; }
  inline const QPoint &position() const { return mPosition; }

private:
  bool mValid = false;
  QPoint mPosition;
};

class BaseTileScene : public QGraphicsScene {
  Q_OBJECT

public:
  //! Constructor
  BaseTileScene(QObject *parent = nullptr) : QGraphicsScene(parent) {}

  //! Returns the current brush
  virtual const TileBrush &brush() const = 0;

  //! Returns the tile size
  virtual QSize tileSize() const = 0;

  //! Returns the tile size
  virtual QSize bgTileSize() const = 0;

  //! Returns the current tileset
  virtual Maker::GameTilesetAsset *tileset() const = 0;

  //! Returns a tile
  virtual TileBrushTile getTile(const QPoint &pos) const = 0;

  //! Sets a tile
  inline void setTile(const QPoint &pos, const TileBrushTile &tile) {
    setTiles({PositionedTileBrushTile(pos, tile)});
  }

  //! Sets tiles
  virtual void setTiles(const PositionedTileList &changes) = 0;

  //! Returns the size
  virtual QSize layerSize() const = 0;

  //! Sets a tile
  virtual void commitChanges() = 0;
};

class MapScene : public BaseTileScene {  
  Q_OBJECT

public:
  //! Constructor
  explicit MapScene(const QString &id, QGraphicsView *view, MapEditorContext *context, QObject *parent = nullptr);

  //! Deconstructor
  ~MapScene();

  //! Returns the ID
  inline const QString &id() const { return mId; }

  //! Whether the scene is valid
  inline bool valid() const { return mTileset != nullptr; }

  //! Returns the tileset
  Maker::GameTilesetAsset *tileset() const override { return mTileset; }

  //! Returns the tilemap
  inline Maker::GameTilemapAsset *tilemap() const { return mTilemap; }

  //! Returns the level
  inline Maker::GameLevelAsset *level() const { return mLevel; }

  //! Returns all layers
  inline MapSceneLayerList *layers() const { return mLayers; }

  //! Returns the map context
  inline MapEditorContext *context() const { return mContext; }

  //! Returns if the map is modified
  bool isModified() const { return mChanged; }

  //! Returns the current brush
  const TileBrush &brush() const override;

  //! Returns the undo stack
  inline QUndoStack *undoStack() const { return mUndoStack; };

  //! Returns a tile
  TileBrushTile getTile(const QPoint &pos) const override;

  //! Sets a tile
  void setTiles(const PositionedTileList &changes) override;

  //! Returns the tile size
  QSize tileSize() const override;

  //! Returns the tile size
  QSize bgTileSize() const override;

  //! Returns the size
  QSize layerSize() const override;

  //! Sets a tile
  void commitChanges() override;

signals:
  //! Hover changed
  void hoverChanged(const MapSceneHoverInfo &);

  //! Notifies that the map has been changed
  void mapChanged();

  //! Modified status changed
  void modifiedChanged(bool);

public slots:
  //! Redraws the full tilemap
  void redrawTilemap();

  //! Redraws a part of a tilemap
  void redrawTilemap(const QRect &rect);

  //! Loads the map
  void unload();

  //! Loads the map
  bool load();

  //! Saves the map
  bool save();

  //! Sets the grid enabled
  void setGridEnabled(bool);

  //! Updates the tool
  void updateTool();

  //! On change
  void onChanged();

protected:
  //! Mouse move
  void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
  
  //! Mouse move
  void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

  //! Mouse move
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

  //! Special events
  bool event(QEvent *event) override;

private:
  //! Visible layers
  MapSceneLayerList *mLayers;

  //! The ID
  QString mId;

  //! The view
  QGraphicsView *mView;

  //! The context
  MapEditorContext *mContext;

  //! The undo stack
  QUndoStack *mUndoStack;

  //! The level
  Maker::GameLevelAsset *mLevel;

  //! The tilemap
  Maker::GameTilemapAsset *mTilemap;

  //! The tileset
  Maker::GameTilesetAsset *mTileset;

  //! The pixmap object
  QGraphicsPixmapItem *mTilemapItem;

  //! The background rect
  QGraphicsRectItem *mBackgroundRect;

  //! The current render pixmap
  QPixmap mTilemapPixmap;
  
  //! The grid
  AobaGrid *mGrid;

  //! Current position
  QPoint mCurrentTile;

  //! Whether the map has been changed
  bool mChanged = false;

  //! The current tool
  MapSceneTool *mTool = nullptr;

  //! Whether the mouse is on the scene
  bool mMouseOnScene = false;
};
