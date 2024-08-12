#pragma once

#include <QSize>
#include <QRect>

class TileSelection {
public:
  //! Constructor
  TileSelection(const QSize &mapSize, const QSize &tileSize)
    : mMapSize(mapSize), mTileSize(tileSize) {}

  //! Sets the map size
  inline void setMapSize(const QSize &size) { mMapSize = size; }

  //! Sets the tile size
  inline void setTileSize(const QSize &size) { mTileSize = size; }

  //! Returns the selection
  inline const QRect &selection() const { return mCurrentSelection; }

  //! Returns the selection
  inline const QRect &lastSelection() const { return mLastSelection; }

  //! Returns the selection
  inline const QRect &confirmedSelection() const { return mLastSelection; }

  //! Moves the current selection
  void move(int x, int y);

  //! Sets the current selection
  void setSelection(const QRect &);

  //! Cancels the selection
  void cancel();

  //! Starts the movement
  bool startSelection(const QPointF &);

  //! Moves the selection
  bool moveSelection(const QPointF &);

  //! Stops the selection
  bool stopSelection(const QPointF &);

private:
  //! Sets the current selection rect
  void updateCurrentSelection();

  //! The map size
  QSize mMapSize;

  //! The tile size
  QSize mTileSize;

  //! Whether the left mouse button is clicked
  bool mIsSelecting = false;

  //! Whether the selection should be cancelled
  bool mCancel = false;

  //! The current selection
  QRect mCurrentSelection;

  //! The last selection
  QRect mLastSelection;

  //! Current selection rect
  QPoint mSelectionStart;

  //! Current selection rect
  QPoint mSelectionCurrent;
};
