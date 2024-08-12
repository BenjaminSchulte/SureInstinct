#include <maker/tileset/TileSelection.hpp>

// -----------------------------------------------------------------------------
void TileSelection::setSelection(const QRect &rect) {
  mLastSelection = rect;
  mCurrentSelection = rect;
}

// -----------------------------------------------------------------------------
void TileSelection::cancel() {
  if (!mIsSelecting) {
    return;
  }

  mCurrentSelection = mLastSelection;
  mCancel = true;
}

// -----------------------------------------------------------------------------
void TileSelection::move(int x, int y) {
  QRect newSelection = QRect(
    QPoint(mCurrentSelection.x() + x, mCurrentSelection.y() + y),
    mCurrentSelection.size()
  ).intersected(QRect(QPoint(0, 0), mMapSize));

  if (newSelection.isValid()) {
    mCurrentSelection = mLastSelection = newSelection;
  }
}

// -----------------------------------------------------------------------------
bool TileSelection::startSelection(const QPointF &point) {
  mCancel = false;
  mSelectionStart = QPoint(point.x() / mTileSize.width(), point.y() / mTileSize.height());
  mSelectionCurrent = mSelectionStart;
  updateCurrentSelection();
  return true;
}

// -----------------------------------------------------------------------------
bool TileSelection::moveSelection(const QPointF &point) {
  if (mCancel) {
    return false;
  }

  mSelectionCurrent = QPoint(point.x() / mTileSize.width(), point.y() / mTileSize.height());
  updateCurrentSelection();
  return true;
}

// -----------------------------------------------------------------------------
bool TileSelection::stopSelection(const QPointF &point) {
  if (mCancel) {
    return false;
  }

  mSelectionCurrent = QPoint(point.x() / mTileSize.width(), point.y() / mTileSize.height());
  updateCurrentSelection();
  mLastSelection = mCurrentSelection;
  return true;
}

// -----------------------------------------------------------------------------
void TileSelection::updateCurrentSelection() {
  QPoint topLeft = QPoint(
    qMax(0, qMin(mSelectionStart.x(), mSelectionCurrent.x())),
    qMax(0, qMin(mSelectionStart.y(), mSelectionCurrent.y()))
  );

  QPoint bottomRight = QPoint(
    qMin(mMapSize.width() - 1,  qMax(mSelectionStart.x(), mSelectionCurrent.x())),
    qMin(mMapSize.height() - 1, qMax(mSelectionStart.y(), mSelectionCurrent.y()))
  );

  mCurrentSelection = QRect(topLeft, bottomRight);
}
