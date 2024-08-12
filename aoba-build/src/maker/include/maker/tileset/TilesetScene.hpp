#pragma once

#include <QMap>
#include <QGraphicsScene>
#include "TileSelection.hpp"
#include "TileBrush.hpp"

class TilemapReader;

namespace Aoba {
class TilesetLayer;
class TilemapAssetSetLayer;
}

namespace Rpg {
class TilesetCollisionModuleInstance;
}

class AobaGrid;

struct TilesetSceneLayer {
  TilesetSceneLayer() {}

  QPixmap pixmap;
};

class TilesetScene : public QGraphicsScene {
  Q_OBJECT

public:
  //! Constructor
  explicit TilesetScene(TilemapReader *context, QObject *parent = nullptr);

  //! Deconstructor
  ~TilesetScene();

  //! Sets whether the grid should be visible
  void setGridVisible(bool visible);

  //! Sets whether collision should be visible
  void setCollisionVisible(bool visible);

  //! Sets whether Exit should be visible
  void setExitVisible(bool visible);

  //! Sets whether Movement should be visible
  void setMovementVisible(bool visible);

  //! Sets whether Enter should be visible
  void setEnterVisible(bool visible);

  //! Returns the tile size
  QSize tileSize() const;

  //! Returns the tile size
  QSize bgTileSize() const;

  //! Returns the current brush
  inline const TileBrush &brush() const { return mTileBrush; }

signals:
  void selectionChanged(const TileBrush &);

public slots:
  void selectTile(int x, int y);

  //! The layer has been changed
	void onLayerChange();

  //! The Tileset has been changed
	void onTilesetChange();

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

  //! Renders tile
  virtual void renderTiles(TilesetSceneLayer &, Aoba::TilesetLayer *, const QRect &);

private:
  //! Renders collision
  void renderCollision(Aoba::TilesetLayer *, Rpg::TilesetCollisionModuleInstance *col);

  //! Commits the selection
  void setSelectionFromRect();

  //! Updates the selection rect
  void updateSelectionRect();

  //! Clears everything
  void clear();

  //! The context
  TilemapReader *mReader;

  //! List of all layers
  QMap<Aoba::TilesetLayer*, TilesetSceneLayer> mLayers;

  //! The current layer
  Aoba::Tileset *mTileset = nullptr;

  //! The current layer
  Aoba::TilesetLayer *mLayer = nullptr;

  //! The background rect
  QGraphicsRectItem *mBackgroundRect;

  //! The background rect
  QGraphicsRectItem *mSelectionRect;

  //! The tiles
  QGraphicsPixmapItem *mTiles;

  //! The selection
  TileSelection mSelection;

  //! The selection depending on the current layer
  QMap<Aoba::TilesetLayer*, QRect> mLayerSelection;

  //! Additional items
  QVector<QGraphicsItem*> mItems;

  //! The grid
  AobaGrid *mGrid;

  //! Whether collision is visible
  bool mShowCollision = false;

  //! Whether Exit is visible
  bool mShowExit = false;

  //! Whether Movement is visible
  bool mShowMovement = false;

  //! Whether Enter is visible
  bool mShowEnter = false;

  //! Current brush
  TileBrush mTileBrush;
};