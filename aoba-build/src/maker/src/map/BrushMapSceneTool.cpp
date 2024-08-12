#include <aoba/log/Log.hpp>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <maker/map/MapEditorContext.hpp>
#include <maker/map/BrushMapSceneTool.hpp>

// -----------------------------------------------------------------------------
void BrushMapSceneTool::clear() {
  MapSceneTool::clear();
  mSelectionRect = nullptr;
}

// -----------------------------------------------------------------------------
bool BrushMapSceneTool::onMouseDown(const QPoint &pos, QGraphicsSceneMouseEvent *event) {
  if (event->button() != Qt::LeftButton) {
    return false;
  }

  mLeftButtonDown = true;
  mStartPosition = pos;
  return onMouseMove(pos, event);
}
  
// -----------------------------------------------------------------------------
bool BrushMapSceneTool::onMouseUp(const QPoint &, QGraphicsSceneMouseEvent *event) {
  if (event->button() != Qt::LeftButton) {
    return false;
  }

  commit();
  return true;
}

// -----------------------------------------------------------------------------
bool BrushMapSceneTool::onMouseMove(const QPoint &pos, QGraphicsSceneMouseEvent *event) {
  if (mLeftButtonDown && !(event->buttons() & Qt::LeftButton)) {
    commit();
  }

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

  if (mLeftButtonDown) {
    plotAt(pos);
  }

  return mLeftButtonDown;
}

// -----------------------------------------------------------------------------
void BrushMapSceneTool::commit() {
  mScene->commitChanges();
  mLeftButtonDown = false;
}

// -----------------------------------------------------------------------------
void BrushMapSceneTool::plotAt(const QPoint &pos) {
  const auto &brush = mScene->brush();

  int left = pos.x() - mStartPosition.x();
  int top = pos.y() - mStartPosition.y();

  PositionedTileList changes;
  for (int y=0; y<brush.size().height(); y++) {
    for (int x=0; x<brush.size().width(); x++) {
      changes.push_back(PositionedTileBrushTile(
        QPoint(pos.x() + x, pos.y() + y),
        brush.tile(left + x, top + y)
      ));
    }
  }

  mScene->setTiles(changes);
}

// -----------------------------------------------------------------------------
bool BrushMapSceneTool::onMouseLeave() {
  clear();
  return false;
}
