#pragma once

#include <QString>
#include <QVector>
#include <QMap>
#include <QPoint>
#include <QRect>
#include "SpriteEditor.hpp"

namespace Aoba {
struct SpriteAnimation;
}

struct SpriteEditorWriterFrame {
  SpriteEditorWriterFrame() {}
  SpriteEditorWriterFrame(
    const QString &name,
    const QString &sourceFrame,
    const QPoint &originalTopLeft,
    const QPoint &rel,
    const QPoint &topLeft,
    const QPoint &bottomRight
  ) : name(name)
    , sourceFrame(sourceFrame)
    , relativePosition(rel)
    , usedRange(QPoint(0, 0), QSize(1, 1))
    , originalTopLeft(originalTopLeft)
    , freeRange(topLeft, bottomRight)
  {}

  QString name;
  QString sourceFrame;
  QPoint relativePosition;
  QRect usedRange;
  QPoint originalTopLeft;
  QRect freeRange;
};

class SpriteEditorWriter {
public:
  //! Constructor
  SpriteEditorWriter(SpriteEditorSprite *sprite) : mSprite(sprite) {}

  //! Saves
  bool save();

private:
  //! Saves all animations
  bool saveAnimations();

  //! Saves all animations
  bool saveAnimation(const QString &id);

  //! Saves all animations
  bool prepareAnimationsCache();

  //! Saves an animation
  bool prepareAnimationCache(const QString &id);

  //! Summaries frame targets
  QVector<QPoint> summaryFramePoints(const QString &frame, QPoint &topLeft, QPoint &bottomRight, QPoint &originalTopLeft) const;

  //! Checks the image boundary
  bool isValidImageBoundary(const QPoint &topLeft, const QPoint &bottomRight) const;

  //! Returns an unique hash
  QString generateHash(const QString &frame, const QVector<QPoint> &targets, const QPoint &topLeft) const;

  //! Adds a frame
  QString cacheFrame(const QString &id, const Aoba::SpriteAnimation &animation);

  //! Saves a frame
  Aoba::SpriteAnimation saveFrame(const QString &id, const QString &frameId, const Aoba::SpriteAnimation &animation);

  //! Generates a frame
  void generateFrame(const SpriteEditorWriterFrame &frame);

  //! Adds to an existing frame
  bool cacheToExistingFrame(SpriteEditorWriterFrame &frame, const QPoint &pos);

  //! Generates all image groups
  void generateImageGroups();

  //! Generates all image groups
  void generateImageGroupsForAnimation(const QString &id);

  //! Creates an image group
  QString createImageGroup(const QString &id, const QVector<int> &);

  //! The sprite
  SpriteEditorSprite *mSprite;

  //! The new sprite
  SpriteEditorSprite mNewSprite;

  //! List of new frames
  QStringList mGeneratedFrames;

  //! Frame counter
  QMap<QString, int> mFrameCounter;

  //! Animation frame to new frame name
  QMap<QString, QMap<int, QString>> mAnimationFrameToNewFrame;

  //! List of all generated frames
  QMap<QString, QVector<SpriteEditorWriterFrame>> mFrames;

  //! Hash of all image groups
  QMap<QString, QString> mImageGroupList;
};