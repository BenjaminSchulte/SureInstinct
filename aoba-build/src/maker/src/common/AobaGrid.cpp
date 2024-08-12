#include <QPen>
#include <QGraphicsLineItem>
#include <maker/common/AobaGrid.hpp>

// -----------------------------------------------------------------------------
AobaGrid::AobaGrid(const QSize &distance, const QSizeF &size, QGraphicsItem *parent)
  : QGraphicsItemGroup(parent)
  , mDistance(distance)
  , mSize(size)
{
  recreateGrid();
  setOpacity(0.5);
}

// -----------------------------------------------------------------------------
AobaGrid::~AobaGrid() {
  for (QGraphicsLineItem *line : mGridLines) {
    delete line;
  }
}

// -----------------------------------------------------------------------------
void AobaGrid::setDistance(const QSize &distance) {
  if (distance == mDistance) {
    return;
  }

  mDistance = distance;
  recreateGrid();
}

// -----------------------------------------------------------------------------
void AobaGrid::setSize(const QSizeF &size) {
  if (size == mSize) {
    return;
  }

  mSize = size;
  recreateGrid();
}

// -----------------------------------------------------------------------------
void AobaGrid::setSubGridEnabled(bool enabled) {
  if (enabled == mSubGrid) {
    return;
  }

  mSubGrid = enabled;
  recreateGrid();
}

// -----------------------------------------------------------------------------
void AobaGrid::recreateGrid() {
  for (QGraphicsLineItem *line : mGridLines) {
    delete line;
  }

  mGridLines.clear();
  QPen pen(QColor(0, 0, 0, 255), 0.5, Qt::SolidLine);

  for (float y=0; y<=mSize.height(); y+=mDistance.height()) {
    QGraphicsLineItem *line = new QGraphicsLineItem(QLineF(QPointF(0, y), QPointF(mSize.width(), y)));
    line->setPen(pen);
    addToGroup(line);
    mGridLines.push_back(line);
  }
  for (float x=0; x<=mSize.width(); x+=mDistance.width()) {
    QGraphicsLineItem *line = new QGraphicsLineItem(QLineF(QPointF(x, 0), QPointF(x, mSize.height())));
    line->setPen(pen);
    addToGroup(line);
    mGridLines.push_back(line);
  }

  if (mSubGrid) {
    pen = QPen(QColor(0, 0, 0, 128), 0.5, Qt::SolidLine);

    for (float y=mDistance.height()/2; y<=mSize.height(); y+=mDistance.height()) {
      QGraphicsLineItem *line = new QGraphicsLineItem(QLineF(QPointF(0, y), QPointF(mSize.width(), y)));
      line->setPen(pen);
      addToGroup(line);
      mGridLines.push_back(line);
    }
    for (float x=mDistance.width()/2; x<=mSize.width(); x+=mDistance.width()) {
      QGraphicsLineItem *line = new QGraphicsLineItem(QLineF(QPointF(x, 0), QPointF(x, mSize.height())));
      line->setPen(pen);
      addToGroup(line);
      mGridLines.push_back(line);
    }
  }
}
