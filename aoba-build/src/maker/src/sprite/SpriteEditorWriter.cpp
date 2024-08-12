#include <maker/sprite/SpriteEditorWriter.hpp>
#include <maker/sprite/SpriteEditor.hpp>

// -----------------------------------------------------------------------------
bool SpriteEditorWriter::save() {
  generateImageGroups();

  if ( !prepareAnimationsCache()
    || !saveAnimations()
  ) {
    return false;
  }


  Aoba::Sprite *sprite = mSprite->mSprite;
  mNewSprite.mImages = mSprite->mImages;
  sprite->overrideData(mNewSprite.mImages, mNewSprite.mImageGroups, mNewSprite.mFrames, mNewSprite.mAnimations);

  return true;
}

// -----------------------------------------------------------------------------
void SpriteEditorWriter::generateImageGroups() {
  QMapIterator<QString, Aoba::SpriteAnimationFrames> it(mSprite->mAnimations);
  while (it.hasNext()) {
    it.next();
    generateImageGroupsForAnimation(it.key());
  }
}

// -----------------------------------------------------------------------------
void SpriteEditorWriter::generateImageGroupsForAnimation(const QString &id) {
  QVector<int> sharedImages;
  QVector<int> staticImages;

  for (const Aoba::SpriteAnimation &record : mSprite->mAnimations[id]) {
    if (record.isCommand()) {
      continue;
    }

    Aoba::SpriteFrame &frame = mSprite->mFrames[record.frame];
    if (!frame.sharedImage.isEmpty()) {
      for (int id : mSprite->mImageGroups[frame.sharedImage]) {
        if (!sharedImages.contains(id)) { sharedImages.push_back(id); }
      }
    }
    if (!frame.staticImage.isEmpty()) {
      for (int id : mSprite->mImageGroups[frame.staticImage]) {
        if (!staticImages.contains(id)) { staticImages.push_back(id); }
      }
    }
  }

  QString newSharedImagesGroup;
  QString newStaticImagesGroup;
  std::sort(sharedImages.begin(), sharedImages.end());
  std::sort(staticImages.begin(), staticImages.end());
  if (!sharedImages.isEmpty()) {
    newSharedImagesGroup = createImageGroup("shared_" + id, sharedImages);
  }
  if (!staticImages.isEmpty()) {
    newStaticImagesGroup = createImageGroup("static_" + id, staticImages);
  }

  for (const Aoba::SpriteAnimation &record : mSprite->mAnimations[id]) {
    if (record.isCommand()) {
      continue;
    }

    Aoba::SpriteFrame &frame = mSprite->mFrames[record.frame];
    frame.sharedImage = newSharedImagesGroup;
    frame.staticImage = newStaticImagesGroup;
  }
}

// -----------------------------------------------------------------------------
QString SpriteEditorWriter::createImageGroup(const QString &id, const QVector<int> &list) {
  QStringList hashList;
  for (int item : list) {
    hashList.push_back(QString::number(item));
  }

  QString hash = hashList.join('_');
  if (mImageGroupList.contains(hash)) {
    return mImageGroupList[hash];
  }

  QString newId = id + "_" + QString::number(mNewSprite.mImageGroups.size());
  mNewSprite.mImageGroups[newId] = list;
  mImageGroupList.insert(hash, newId);
  return newId;
}

// -----------------------------------------------------------------------------
bool SpriteEditorWriter::saveAnimations() {
  QMapIterator<QString, Aoba::SpriteAnimationFrames> it(mSprite->mAnimations);
  while (it.hasNext()) {
    it.next();
    if (it.value().isEmpty()) {
      continue;
    }

    if (!saveAnimation(it.key())) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool SpriteEditorWriter::saveAnimation(const QString &id) {
  int index = 0;
  QVector<Aoba::SpriteAnimation> result;

  for (const Aoba::SpriteAnimation &record : mSprite->mAnimations[id]) {
    switch (record.command) {
      case Aoba::SpriteAnimationCommand::NONE:
        result.push_back(saveFrame(id, mAnimationFrameToNewFrame[id][index++], record));
        break;

      default:
        result.push_back(record);
        break;
    }
  }

  mNewSprite.mAnimations.insert(id, result);

  return true;
}

// -----------------------------------------------------------------------------
bool SpriteEditorWriter::prepareAnimationsCache() {
  QMapIterator<QString, Aoba::SpriteAnimationFrames> it(mSprite->mAnimations);
  while (it.hasNext()) {
    it.next();

    if (!prepareAnimationCache(it.key())) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool SpriteEditorWriter::prepareAnimationCache(const QString &id) {
  int index = 0;

  for (const Aoba::SpriteAnimation &record : mSprite->mAnimations[id]) {
    switch (record.command) {
      case Aoba::SpriteAnimationCommand::NONE:
        mAnimationFrameToNewFrame[id][index++] = cacheFrame(id, record);
        break;

      default:
        break;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
QVector<QPoint> SpriteEditorWriter::summaryFramePoints(const QString &frame, QPoint &topLeft, QPoint &bottomRight, QPoint &originalTopLeft) const {
  QVector<QPoint> targets;

  for (const auto &imageInstance : mSprite->mFrames[frame].images) {
    const auto &image = mSprite->mImages[imageInstance.image];

    QPoint target(imageInstance.x - image.pivotX, imageInstance.y - image.pivotY);
    targets.push_back(target);

    topLeft.setX(qMin(topLeft.x(), target.x()));
    topLeft.setY(qMin(topLeft.y(), target.y()));
    originalTopLeft.setX(qMin(-image.pivotX, originalTopLeft.x()));
    originalTopLeft.setY(qMin(-image.pivotY, originalTopLeft.y()));
    bottomRight.setX(qMax(bottomRight.x(), target.x() + image.image->width()));
    bottomRight.setY(qMax(bottomRight.y(), target.y() + image.image->height()));
  }

  return targets;
}

// -----------------------------------------------------------------------------
bool SpriteEditorWriter::isValidImageBoundary(const QPoint &topLeft, const QPoint &bottomRight) const {
  return topLeft.x() < -128 || topLeft.y() < -128 || bottomRight.x() >= 128 || bottomRight.y() >= 128;
}

// -----------------------------------------------------------------------------
QString SpriteEditorWriter::generateHash(const QString &frame, const QVector<QPoint> &targets, const QPoint &topLeft) const {
  QStringList alignedPointList;
  alignedPointList << mSprite->mFrames[frame].sharedImage;
  alignedPointList << mSprite->mFrames[frame].staticImage;

  int index = 0;
  for (const auto &imageInstance : mSprite->mFrames[frame].images) {
    const QPoint &point = targets[index++];
    alignedPointList << QString::number(imageInstance.image) << QString::number(imageInstance.priority);
    alignedPointList << QString::number(point.x() - topLeft.x()) << QString::number(point.y() - topLeft.y());
  }
  return alignedPointList.join('$');
}

// -----------------------------------------------------------------------------
bool SpriteEditorWriter::cacheToExistingFrame(SpriteEditorWriterFrame &frame, const QPoint &pos) {
  QPoint relPos = pos - frame.relativePosition;

  if (frame.usedRange.contains(relPos)) {
    return true;
  }
  
  if (frame.freeRange.contains(relPos)) {
    frame.usedRange = frame.usedRange.united(QRect(relPos, QSize(1, 1)));
    return true;
  }

  int rightAlign = frame.freeRange.right() - frame.usedRange.right();
  int leftAlign = frame.usedRange.left() - frame.freeRange.left();
  int bottomAlign = frame.freeRange.bottom() - frame.usedRange.bottom();
  int topAlign = frame.usedRange.top() - frame.freeRange.top();

  bool validY = QRect(QPoint(0, frame.freeRange.top()), QPoint(0, frame.freeRange.bottom())).contains(QPoint(0, relPos.y()));
  bool validX = QRect(QPoint(0, frame.freeRange.left()), QPoint(0, frame.freeRange.bottom())).contains(QPoint(0, relPos.x()));

  QPoint shift;
  if (validY) {
  } else if (relPos.y() < 0 && relPos.y() >= frame.freeRange.top() - bottomAlign) {
    shift.setY(relPos.y() - frame.freeRange.top());
  } else if (relPos.y() > 0 && relPos.y() <= frame.freeRange.bottom() + topAlign) {
    shift.setY(relPos.y() - frame.freeRange.bottom());
  } else {
    return false;
  }
  if (validX) {
  } else if (relPos.x() < 0 && relPos.x() >= frame.freeRange.left() - rightAlign) {
    shift.setX(relPos.x() - frame.freeRange.left());
  } else if (relPos.x() > 0 && relPos.x() <= frame.freeRange.right() + leftAlign) {
    shift.setX(relPos.x() - frame.freeRange.right());
  } else {
    return false;
  }

  frame.relativePosition += shift;
  frame.usedRange.setTopLeft(frame.usedRange.topLeft() - shift);
  frame.usedRange.setBottomRight(frame.usedRange.bottomRight() - shift);
  relPos -= shift;
  frame.usedRange = frame.usedRange.united(QRect(relPos, QSize(1, 1)));

  return true;
}

// -----------------------------------------------------------------------------
QString SpriteEditorWriter::cacheFrame(const QString &id, const Aoba::SpriteAnimation &animation) {
  QPoint topLeft(10000, 10000);
  QPoint originalTopLeft(10000, 10000);
  QPoint bottomRight(-10000, -10000);

  QVector<QPoint> targets = summaryFramePoints(animation.frame, topLeft, bottomRight, originalTopLeft);

  // Checks the boundaries basically
  if (isValidImageBoundary(topLeft, bottomRight)) {
    Aoba::log::warn("Animation frame image boundary exceeded");
    return "";
  }

  // Generates a hash 
  QString hash(generateHash(animation.frame, targets, topLeft));

  // Search for an existing frame which fits our requirements
  if (mFrames.contains(hash)) {
    for (SpriteEditorWriterFrame &frame : mFrames[hash]) {
      if (cacheToExistingFrame(frame, topLeft + QPoint(animation.x, animation.y))) {
        return frame.name;
      }
    }
  }
  
  // Checks how many we can adjust the frame
  QPoint freeTopLeft(
    -qMin(8, topLeft.x() + 128),
    -qMin(8, topLeft.y() + 128)
  );
  QPoint freeBottomRight(
    qMin(7, 128 - bottomRight.x()),
    qMin(7, 128 - bottomRight.y())
  );

  // We need to add a new frame
  QString frameId = id + "_" + QString::number(mFrameCounter[id]++);
  mFrames[hash].push_back(SpriteEditorWriterFrame(
    frameId, animation.frame, originalTopLeft, topLeft, freeTopLeft, freeBottomRight
  ));
  return frameId;
}

// -----------------------------------------------------------------------------
void SpriteEditorWriter::generateFrame(const SpriteEditorWriterFrame &frame) {
  QPoint topLeft(10000, 10000);
  QPoint originalTopLeft(10000, 10000);
  QPoint bottomRight(-10000, -10000);
  QVector<QPoint> targets = summaryFramePoints(frame.sourceFrame, topLeft, bottomRight, originalTopLeft);

  Aoba::SpriteFrame result = mSprite->mFrames[frame.sourceFrame];
  QPoint shift = frame.relativePosition - topLeft;

  for (auto &image : result.images) {
    image.x += shift.x();
    image.y += shift.y();
  }

  mGeneratedFrames.push_back(frame.name);
  mNewSprite.mFrames.insert(frame.name, result);
}

// -----------------------------------------------------------------------------
Aoba::SpriteAnimation SpriteEditorWriter::saveFrame(const QString &id, const QString &frameId, const Aoba::SpriteAnimation &animation) {
  QPoint topLeft(10000, 10000);
  QPoint originalTopLeft(10000, 10000);
  QPoint bottomRight(-10000, -10000);
  QVector<QPoint> targets = summaryFramePoints(animation.frame, topLeft, bottomRight, originalTopLeft);

  // Generates a hash 
  QString hash(generateHash(animation.frame, targets, topLeft));
  if (!mFrames.contains(hash)) {
    Aoba::log::warn("Unexpected error in saveFrame " + id);
    return Aoba::SpriteAnimation();
  }

  for (const SpriteEditorWriterFrame &frame : mFrames[hash]) {
    if (frame.name != frameId) {
      continue;
    }

    if (!mGeneratedFrames.contains(frameId)) {
      generateFrame(frame);
    }

    QPoint pos = (topLeft + QPoint(animation.x, animation.y)) - frame.relativePosition;
    return Aoba::SpriteAnimation(frameId, pos.x(), pos.y(), animation.flipX, animation.flipY, animation.delay);
  }

  Aoba::log::warn("Unexpected error in saveFrame 2");
  return Aoba::SpriteAnimation();
}