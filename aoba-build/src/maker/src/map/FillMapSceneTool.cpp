#include <aoba/log/Log.hpp>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <maker/map/MapEditorContext.hpp>
#include <maker/map/FillMapSceneTool.hpp>

// -----------------------------------------------------------------------------
void FillMapSceneTool::clear() {
  MapSceneTool::clear();
  mSelectionRect = nullptr;
}

// -----------------------------------------------------------------------------
bool FillMapSceneTool::onMouseDown(const QPoint &pos, QGraphicsSceneMouseEvent *event) {
  if (event->button() != Qt::LeftButton) {
    return false;
  }

  fillAt(pos);

  onMouseMove(pos, event);
  return true;
}

// -----------------------------------------------------------------------------
bool FillMapSceneTool::onMouseMove(const QPoint &pos, QGraphicsSceneMouseEvent *) {
  if (!mSelectionRect) {
    QPen pen(QColor(255, 255, 0, 255));

    mSelectionRect = new QGraphicsRectItem(QRect(0, 0, 0, 0));
    mSelectionRect->setPen(pen);
    mSelectionRect->setZValue(100);

    add(mSelectionRect);
  }

  const TileBrush &brush = mScene->brush();
  QSize tileSize(mScene->tileSize());
  mSelectionRect->setRect(QRect(
    pos.x() * tileSize.width(),
    pos.y() * tileSize.height(),
    brush.size().width() * tileSize.width(),
    brush.size().height() * tileSize.height()
  ));

  return false;
}

// -----------------------------------------------------------------------------
void FillMapSceneTool::fillAt(const QPoint &pos) {
  const TileBrush &brush = mScene->brush();
  QSize size = mScene->layerSize();

  bool *filled = new bool[size.width() * size.height()];
  memset(filled, 0, sizeof(bool) * size.width() * size.height());

  QVector<QPoint> startPositions;
  startPositions.push_back(pos);

  PositionedTileList changes;
  TileBrushTile originalTile(mScene->getTile(pos));
  while (!startPositions.isEmpty()) {
    QPoint point(startPositions.back());
    startPositions.pop_back();

    if (point.x() < 0 || point.y() < 0 || point.x() >= size.width() || point.y() >= size.height()) {
      continue;
    }

    int index = point.x() + point.y() * size.width();
    if (filled[index]) {
      continue;
    }

    if (mScene->getTile(point) != originalTile) {
      continue;
    }

    filled[index] = true;
    startPositions.push_back(QPoint(point.x() - 1, point.y()));
    startPositions.push_back(QPoint(point.x() + 1, point.y()));
    startPositions.push_back(QPoint(point.x(), point.y() - 1));
    startPositions.push_back(QPoint(point.x(), point.y() + 1));
    changes.push_back(PositionedTileBrushTile(point, brush.tile(point - pos)));
  }

  mScene->setTiles(changes);
  mScene->commitChanges();

  delete[] filled;
}

// -----------------------------------------------------------------------------
bool FillMapSceneTool::onMouseLeave() {
  clear();
  return false;
}
