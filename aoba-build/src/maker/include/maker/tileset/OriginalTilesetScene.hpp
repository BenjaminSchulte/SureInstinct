#pragma once

#include <QPixmap>
#include <QGraphicsScene>
#include <maker/tileset/TileSelection.hpp>

namespace Aoba {
class TilesetTiles;
}

class AobaGrid;

class OriginalTilesetScene : public QGraphicsScene {
  Q_OBJECT

public:
  //! Constructor
  OriginalTilesetScene(QObject *parent);

  //! Sets the current tileset layer
  void setLayer(Aoba::TilesetTiles *layer);

  //! Returns the selection
  inline const QRect &selection() const { return mSelection; }

  //! Sets the selection
  void setSelection(const QRect &selection);

  //! Returns the tile size
  QSize tileSize() const;

  //! Returns the tile size
  QSize bgTileSize() const;

signals:
  void selectionChanged(const QRect &);

public slots:
  //! Redraws the layer
  void redrawLayer();

  //! Redraws the layer
  void redrawLayer(const QRect &tiles);

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
  //! Updates the selection rect
  void updateSelectionRect();

  //! The grid
  AobaGrid *mGrid;

  //! The current layer
  Aoba::TilesetTiles *mLayer = nullptr;

  //! The current selection
  QRect mSelection;

  //! The selection
  TileSelection mSelectionHelper;

  //! The tiles
  QGraphicsPixmapItem *mTiles;

  //! The background rect
  QGraphicsRectItem *mSelectionRect;

  //! The background rect
  QGraphicsRectItem *mBackgroundRect;

  //! The tiles pixmap
  QPixmap mPixmap;
};