#include <aoba/log/Log.hpp>
#include <QPainter>
#include <QPixmap>
#include <aoba/sprite/Sprite.hpp>
#include <maker/sprite/SpriteEditor.hpp>
#include <maker/sprite/SpriteAnimationScene.hpp>

// -----------------------------------------------------------------------------
SpriteAnimationScene::SpriteAnimationScene(QObject *parent)
  : QGraphicsScene(parent)
{
  mAnimationTimer.setInterval(1000 / 50.0);

  connect(&mAnimationTimer, &QTimer::timeout, this, &SpriteAnimationScene::nextFrame);

  recreateGrid();
}

// -----------------------------------------------------------------------------
SpriteAnimationScene::~SpriteAnimationScene() {
}

// -----------------------------------------------------------------------------
void SpriteAnimationScene::restart() {
  mNextFrame = 0;
  mFrameDelay = 0;
  mLoopStart = 0;
  mMoveSpeed = QPointF(0, 0);
}

// -----------------------------------------------------------------------------
void SpriteAnimationScene::setPaused(bool paused) {
  mPaused = paused;

  if (paused) {
    mAnimationTimer.stop();
  } else if (mSprite) {
    mAnimationTimer.start();
  }
}

// -----------------------------------------------------------------------------
void SpriteAnimationScene::recreateGrid() {
  clear();

  QPen hardPen(QColor(164,164,164,255));
  QPen softPen(QColor(224,224,224,255));
  
  QPixmap gridPattern(16, 16);
  gridPattern.fill(QColor(0, 0, 0, 0));
  QPainter gridPainter(&gridPattern);
  gridPainter.setPen(softPen);
  gridPainter.drawLine(0, 0, 16, 0);
  gridPainter.drawLine(0, 0, 0, 16);
  mGrid = addRect(QRect(-1000, -1000, 2000, 2000), QPen(Qt::NoPen), QBrush(gridPattern));

  addLine(QLineF(0.5, -1000, 0.5, 1000), hardPen);
  addLine(QLineF(-1000, 0.5, 1000, 0.5), hardPen);

  setSceneRect(QRect(-100, -100, 200, 200));
}

// -----------------------------------------------------------------------------
void SpriteAnimationScene::clearAnimation() {
  mEditor = nullptr;
  mSprite = nullptr;
  mNextFrame = 0;
  mAnimationTimer.stop();
  mMoveSpeed = QPointF(0, 0);
  mLoopStart = 0;

  mPixmaps.clear();
  recreateGrid();
}

// -----------------------------------------------------------------------------
void SpriteAnimationScene::setAnimation(SpriteEditorSprite *sprite, const QString &anim) {
  mSprite = sprite;
  mAnimation = anim;
  restart();

  if (!mPaused) {
    mAnimationTimer.start();
  }

  recreateGrid();
  nextFrame();
}

// -----------------------------------------------------------------------------
void SpriteAnimationScene::moveGrid(const QPointF &speed) {
  QPointF pos(mGrid->pos() - speed);

  if (pos.x() < -16) { pos.setX(pos.x() + 16.0); }
  else if (pos.x() > 16) { pos.setX(pos.x() - 16.0); }
  if (pos.y() < -16) { pos.setY(pos.y() + 16.0); }
  else if (pos.y() > 16) { pos.setY(pos.y() - 16.0); }

  mGrid->setPos(pos);
}

// -----------------------------------------------------------------------------
void SpriteAnimationScene::nextFrame() {
  if (!mMoveSpeed.isNull()) {
    moveGrid(mMoveSpeed);
  }

  if (mFrameDelay--) {
    return;
  }

  const QVector<Aoba::SpriteAnimation> &frames = mSprite->mAnimations[mAnimation];
  if (frames.size() == 0) {
    mFrameDelay = 15;
    return;
  }

  bool revertedOnce = false;

  while (true) {
    if (mNextFrame >= frames.size() || frames[mNextFrame].isLoopCommand()) {
      if (revertedOnce) {
        mFrameDelay = 15;
        mNextFrame = 0;
        return;
      }

      mNextFrame = mLoopStart;
      if (mLoopStart == 0) {
        mMoveSpeed = QPointF(0, 0);
      }
      revertedOnce = true;
    }

    const Aoba::SpriteAnimation &frame = frames[mNextFrame++];
    if (frame.isCommand()) {
      switch (frame.command) {
        case Aoba::SpriteAnimationCommand::BEGIN_LOOP:
          mLoopStart = mNextFrame - 1;
          break;
        case Aoba::SpriteAnimationCommand::MOVE:
          mMoveSpeed = mMoveDirection * (frame.commandParameter / 256.0);
          break;
        default:
          break;
      }
    } else {
      setFrame(frame);
      break;
    }
  }
}

// -----------------------------------------------------------------------------
void SpriteAnimationScene::clearFrame() {
  for (const auto *item : mPixmaps) {
    delete item;
  }
  mPixmaps.clear();  
  mFrameDelay = 0;
}

// -----------------------------------------------------------------------------
void SpriteAnimationScene::setFrame(const Aoba::SpriteAnimation &frame) {
  const Aoba::SpriteFrame &images = mSprite->mFrames[frame.frame];

  clearFrame();
  mFrameDelay = frame.delay;

  for (const Aoba::SpriteFrameImage &image : images.images) {
    if (image.image >= mSprite->mImages.size()) {
      Aoba::log::warn("Could not find image in sprite " + QString::number(image.image));
      continue;
    }

    const Aoba::SpriteImage &imageInfo = mSprite->mImages[image.image];

    QPoint pos(image.x + frame.x - imageInfo.pivotX, image.y + frame.y - imageInfo.pivotY);
    
    QPixmap pixmap;
    QImagePtr imageData = mSprite->frameImage(image.image);
    pixmap.convertFromImage(*imageData.get());

    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
    item->setTransform(QTransform::fromScale(frame.flipX ? -1 : 1, frame.flipY ? -1 : 1));
    item->setPos(QPoint(frame.flipX ? -pos.x() : pos.x(), frame.flipY ? -pos.y() : pos.y()));
    mPixmaps.push_back(item);
    addItem(item);
  }
}