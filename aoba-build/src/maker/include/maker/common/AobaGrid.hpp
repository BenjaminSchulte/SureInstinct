#pragma once

#include <QGraphicsItemGroup>

class QGraphicsLineItem;

class AobaGrid : public QGraphicsItemGroup {
public:
  //! Constructor
  AobaGrid(const QSize &distance, const QSizeF &size, QGraphicsItem *parent = nullptr);

  //! Deconstructor
  ~AobaGrid();

  //! Sets the distance
  void setDistance(const QSize &distance);

  //! Sets the distance
  void setSize(const QSizeF &size);

  //! Sets a sub grid
  void setSubGridEnabled(bool enabled);

  //! Returns whether the sub grid is enabled
  inline bool isSubGridEnabled() const { return mSubGrid; }

  //! Returns the size
  inline const QSize &distance() const { return mDistance; }

  //! Returns the size
  inline const QSizeF &size() const { return mSize; }

private:
  //! Recreates the grid
  void recreateGrid();

  //! Grid
  QVector<QGraphicsLineItem*> mGridLines;

  //! Whether to use a sub grid
  bool mSubGrid = false;

  //! The distance
  QSize mDistance;

  //! The size
  QSizeF mSize;
};