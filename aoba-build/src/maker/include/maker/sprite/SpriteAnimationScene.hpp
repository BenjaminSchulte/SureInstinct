#pragma once

#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QVector>
#include <QTimer>

struct SpriteEditorSprite;

class SpriteAnimationScene : public QGraphicsScene {
public:
  //! Constructor
  SpriteAnimationScene(QObject *parent);

  //! Deconstructor
  ~SpriteAnimationScene();

  //! Removes the animation
  void clearAnimation();

  //! Clears the frame
  void clearFrame();

  //! Sets an animation
  void setAnimation(SpriteEditorSprite *, const QString &);

  //! Restarts the animation
  void restart();

  //! Sets the animation paused
  void setPaused(bool);

  //! Set move vector
  inline void setMoveVector(const QPointF &direction) { mMoveDirection = direction; }

private slots:
  void nextFrame();

private:
  //! Recreates the grid
  void recreateGrid();

  //! Moves the grid
  void moveGrid(const QPointF &speed);

  //! Sets a frame
  void setFrame(const Aoba::SpriteAnimation &frame);

  //! The current sprite
  SpriteEditor *mEditor = nullptr;

  //! The current sprite
  SpriteEditorSprite *mSprite = nullptr;

  //! The animation
  QString mAnimation;

  //! The next frame
  int mNextFrame;

  //! Current frame delay
  int mFrameDelay = 0;

  //! Loop start frame
  int mLoopStart = 0;

  //! The grid
  QGraphicsRectItem *mGrid = nullptr;

  //! List of current graphics
  QVector<QGraphicsPixmapItem*> mPixmaps;

  //! The animation timer
  QTimer mAnimationTimer;

  //! The move direction
  QPointF mMoveDirection;

  //! The move speed
  QPointF mMoveSpeed;

  //! Whether the animation is paused
  bool mPaused = false;
};