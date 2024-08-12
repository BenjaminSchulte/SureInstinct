#pragma once

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QVector>

typedef QSharedPointer<QImage> QImagePtr;

class SpriteEditorSceneItem : public QGraphicsPixmapItem {
public:
  //! Constructor
  SpriteEditorSceneItem(int imageIndex, const QPixmap &pixmap, bool isStatic, bool isShared, bool priority);

  //! Returns the image index
  inline int imageIndex() const { return mImageIndex; }

  //! Returns the priority flag
  inline bool isStatic() const { return mStatic; }

  //! Returns the priority flag
  inline bool isShared() const { return mShared; }

  //! Returns the priority flag
  inline bool priority() const { return mPriority; }

  //! Returns the priority flag
  inline void setIsStatic(bool value) { mStatic = value; }

  //! Returns the priority flag
  inline void setIsShared(bool value) { mShared = value; }

  //! Returns the priority flag
  inline void setPriority(bool value) { mPriority = value; }

  //! Returns the position for a flip
  inline QPoint flippedPos(bool x, bool y) const {
    return QPoint(x ? -pos().x() : pos().x(), y ? -pos().y() : pos().y());
  }

protected:
  //! Change value
  QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private:
  //! The image index
  int mImageIndex;

  //! The priority flag
  bool mStatic;

  //! The priority flag
  bool mShared;

  //! The priority flag
  bool mPriority;
};


class SpriteEditorScene : public QGraphicsScene {
  Q_OBJECT

public:
  //! Constructor
  SpriteEditorScene(QObject *parent);

  //! Deconstructor
  ~SpriteEditorScene();

  //! Removes all images
  void removeAllImages();

  //! Sets frame flipping
  void setFlip(bool x, bool y);

  //! Adds an image
  void addImage(int index, const QImagePtr &image, const QPoint &pos, bool isStatic, bool isShared, bool priority);

  //! Returns all images
  inline const QVector<SpriteEditorSceneItem*> &images() const { return mItems; }

  //! Returns whether this is flipped
  inline bool isFlipX() const { return mFlipX; }

  //! Returns whether this is flipped
  inline bool isFlipY() const { return mFlipY; }

signals:
  //! Any image has been changed
  void imagesChanged();

public slots:
  //! Item did move
  void onItemMoved(SpriteEditorSceneItem *item);

  //! Deletes the current item
  void deleteCurrentItem();

protected:
  void keyPressEvent(QKeyEvent *event);

private:
  //! List of all items
  QVector<SpriteEditorSceneItem*> mItems;

  //! Flip X
  bool mFlipX = false;

  //! Flip Y
  bool mFlipY = false;
};