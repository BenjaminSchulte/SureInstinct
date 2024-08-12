#pragma once

#include <QPixmap>
#include <QGraphicsScene>

class AobaGrid;
class TilesetEditorTilesetLayer;

class TilesetEditorScene : public QGraphicsScene {
  Q_OBJECT

public:
  //! Constructor
  TilesetEditorScene(QObject *parent);

  //! Deconstructor
  ~TilesetEditorScene();

  //! Returns the tile size
  QSize bgTileSize() const;

  //! Returns the tile size
  QSize tileSize() const;

  //! Sets the current tileset layer
  void setLayer(TilesetEditorTilesetLayer *layer);

  //! Sets the grid size
  void setToolGrid(int size);

  //! Sets the brush size
  void setBrushSize(const QSize &size);

  //! Sets the priority visibility
  void setCollisionVisible(bool visible);

  //! Sets the priority visibility
  void setPriorityVisible(bool visible);
  
  //! Sets whether the movement is visible
  void setMovementVisible(bool visible);
  
  //! Sets whether the event is visible
  void setEventVisible(bool visible);

  //! Sets whether the event is visible
  void setExitVisible(bool visible);

  //! Sets whether the event is visible
  void setGridVisible(bool visible);

signals:
  void editStarted(const QRect &);
  void editTriggered(const QRect &);
  void editFinished();

public slots:
  //! Redraws the layer
  void redrawLayer();

  //! Redraws the layer
  void redrawLayer(const QRect &tiles);

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

  //! Special events
  bool event(QEvent *event) override;

private:
  //! Clears the selection
  void clear();

  //! Clears the selection
  void clearRect(const QRect &);

  //! Adds an item
  void addItemTo(int x, int y, QGraphicsItem*);

  //! Adds an item and return it
  template<typename T>
  T *addTypeItemTo(int x, int y, T* item) { addItemTo(x, y, item); return item; }

  //! Returns the position
  QRect eventToSelection(QGraphicsSceneMouseEvent *event) const;

  //! The grid
  AobaGrid *mGrid;

  //! Grid size
  int mGridSize = 16;

  //! The current layer
  TilesetEditorTilesetLayer *mLayer = nullptr;

  //! The tiles
  QGraphicsPixmapItem *mTiles;

  //! The background rect
  QGraphicsRectItem *mSelectionRect;

  //! The background rect
  QGraphicsRectItem *mBackgroundRect;

  //! The tiles pixmap
  QPixmap mPixmap;

  //! The current selection
  QRect mSelection;

  //! Whether the user is painting
  bool mIsPainting = false;

  //! Whether the priority layer is visible
  bool mCollisionVisible = false;

  //! Whether the priority layer is visible
  bool mPriorityVisible = false;

  //! Whether the priority layer is visible
  bool mMovementVisible = false;

  //! Whether the priority layer is visible
  bool mEventVisible = false;

  //! Whether the priority layer is visible
  bool mExitVisible = false;

  //! The brush size
  QSize mBrushSize;

  //! Custom graphic items
  QMap<int, QGraphicsItem*> mItems;
};