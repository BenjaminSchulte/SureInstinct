#include <QGraphicsItem>
#include <maker/map/MapSceneLayer.hpp>
#include <maker/map/MapScene.hpp>

namespace {
  int nextLayerId = 1;
}

TileBrush MapSceneLayer::mInvalidBrush;

// -----------------------------------------------------------------------------
MapSceneLayerRenderInfo::MapSceneLayerRenderInfo(MapScene *scene, const QRect &rect)
  : mScene(scene)
  , mTileRect(rect)
{
  QSize tileSize(scene->tileSize());
  mRenderRect = QRect(
    rect.left() * tileSize.width(), rect.top() * tileSize.height(),
    rect.width() * tileSize.width(), rect.height() * tileSize.height()
  );

  mMainPixmap = QPixmap(mRenderRect.size());
  mSubPixmap = QPixmap(mRenderRect.size());
  mOverlayPixmap = QPixmap(mRenderRect.size());

  mMainPixmap.fill(QColor(0, 0, 0, 0));
  mSubPixmap.fill(QColor(0, 0, 0, 0));
  mOverlayPixmap.fill(QColor(0, 0, 0, 0));
}

// -----------------------------------------------------------------------------
MapSceneLayer::MapSceneLayer(const QString &name)
  : mId(nextLayerId++)
  , mName(name)
{
}

// -----------------------------------------------------------------------------
void MapSceneLayer::setVisible(bool visible) {
  if (visible == mVisible) {
    return;
  }

  mVisible = visible;
  if (!visible) {
    clearSceneItems();
  }

  emit visibleChanged();
}

// -----------------------------------------------------------------------------
void MapSceneLayer::clearSceneItems() {
  for (QGraphicsItem *item : mSceneItems) {
    delete item;
  }
  mSceneItems.clear();
}

// -----------------------------------------------------------------------------
void MapSceneLayer::addSceneItem(MapSceneLayerRenderInfo &render, QGraphicsItem *item) {
  if (item == nullptr) {
    return;
  }

  render.mapScene()->addItem(item);
  mSceneItems.push_back(item);
}
