#include <QGraphicsItem>
#include <maker/map/MapSceneTool.hpp>

// -----------------------------------------------------------------------------
void MapSceneTool::clear() {
  for (QGraphicsItem *item : mItems) {
    delete item;
  }
  mItems.clear();
}

// -----------------------------------------------------------------------------
void MapSceneTool::add(QGraphicsItem *item) {
  if (mScene) {
    mScene->addItem(item);
  }
  mItems.push_back(item);
}