#include <aoba/log/Log.hpp>
#include <QPixmap>
#include <QKeyEvent>
#include <QGraphicsPixmapItem>
#include <maker/sprite/SpriteEditorScene.hpp>

// -----------------------------------------------------------------------------
SpriteEditorSceneItem::SpriteEditorSceneItem(int index, const QPixmap &pixmap, bool isStatic, bool isShared, bool priority)
  : QGraphicsPixmapItem(pixmap)
  , mImageIndex(index)
  , mStatic(isStatic)
  , mShared(isShared)
  , mPriority(priority)
{
  setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemSendsGeometryChanges);
}

// -----------------------------------------------------------------------------
QVariant SpriteEditorSceneItem::itemChange(GraphicsItemChange change, const QVariant &value) {
  if (change == ItemPositionChange) {
    QPointF pos = value.toPointF();
    return QGraphicsPixmapItem::itemChange(change, QPointF(QPoint(pos.x(), pos.y())));
  } else if (change == ItemPositionHasChanged && scene()) {
    dynamic_cast<SpriteEditorScene*>(scene())->onItemMoved(this);
  }

  return QGraphicsPixmapItem::itemChange(change, value);
}




// -----------------------------------------------------------------------------
SpriteEditorScene::SpriteEditorScene(QObject *parent) 
  : QGraphicsScene(parent)
{
  addLine(QLineF(-100000, 0.5, 100000, 0.5), QPen(QColor(164, 164, 164, 255)));
  addLine(QLineF(0.5, -100000, 0.5, 100000), QPen(QColor(164, 164, 164, 255)));

  setSceneRect(QRect(-150, -150, 300, 300));
}

// -----------------------------------------------------------------------------
SpriteEditorScene::~SpriteEditorScene() {
  removeAllImages();
}

// -----------------------------------------------------------------------------
void SpriteEditorScene::onItemMoved(SpriteEditorSceneItem *) {
  emit imagesChanged();
}

// -----------------------------------------------------------------------------
void SpriteEditorScene::deleteCurrentItem() {
  if (selectedItems().isEmpty()) {
    return;
  }

  for (QGraphicsItem *obj : selectedItems()) {
    SpriteEditorSceneItem *item = dynamic_cast<SpriteEditorSceneItem*>(obj);
    if (!item) {
      continue;
    }

    mItems.removeAll(item);
    delete item;
  }

  emit imagesChanged();
}

// -----------------------------------------------------------------------------
void SpriteEditorScene::keyPressEvent(QKeyEvent *event) {
  switch (event->key()) {
    case Qt::Key_Delete:
      deleteCurrentItem();
      break;

    default:
      QGraphicsScene::keyPressEvent(event);
      break;
  }
}

// -----------------------------------------------------------------------------
void SpriteEditorScene::setFlip(bool x, bool y) {
  bool changeX = (mFlipX != x);
  bool changeY = (mFlipY != y);

  for (SpriteEditorSceneItem *item : mItems) {
    QPoint newPos = item->flippedPos(changeX, changeY);
    item->setTransform(QTransform::fromScale(x ? -1 : 1, y ? -1 : 1));
    item->setPos(newPos);
  }

  mFlipX = x;
  mFlipY = y;
}

// -----------------------------------------------------------------------------
void SpriteEditorScene::addImage(int index, const QImagePtr &image, const QPoint &pos, bool isStatic, bool isShared, bool priority) {
  QPoint newPos = pos;

  QPixmap pixmap;
  pixmap.convertFromImage(*image.get());
  SpriteEditorSceneItem *item = new SpriteEditorSceneItem(index, pixmap, isStatic, isShared, priority);
  item->setTransform(QTransform::fromScale(mFlipX ? -1 : 1, mFlipY ? -1 : 1));

  if (mFlipX) {
    newPos.setX(0 - newPos.x());
  }
  if (mFlipY) {
    newPos.setY(0 - newPos.y());
  }

  item->setPos(newPos.x(), newPos.y());
  addItem(item);

  mItems.push_back(item);
}

// -----------------------------------------------------------------------------
void SpriteEditorScene::removeAllImages() {
  for (const auto *item : mItems) {
    delete item;
  }
  mItems.clear();
}