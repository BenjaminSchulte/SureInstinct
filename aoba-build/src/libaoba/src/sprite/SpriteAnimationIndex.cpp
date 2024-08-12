#include <aoba/sprite/SpriteAnimationIndex.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
SpriteAnimationIndex::SpriteAnimationIndex(SpriteAnimationIndexAssetSet *set, const QString &id)
  : Asset(set, id)
{
  set->add(this);
}

// -----------------------------------------------------------------------------
int SpriteAnimationIndexAssetSet::numDirections() const {
  if (mParent) {
    return mParent->numDirections() + mDirections.size();
  } else {
    return mDirections.size();
  }
}

// -----------------------------------------------------------------------------
int SpriteAnimationIndexAssetSet::numAnimations() const {
  if (mParent) {
    return mParent->numAnimations() + mAnimations.size();
  } else {
    return mAnimations.size();
  }
}

// -----------------------------------------------------------------------------
int SpriteAnimationIndexAssetSet::numFrameGroups() const {
  if (mParent) {
    return mParent->numFrameGroups() + mFrameGroups.size();
  } else {
    return mFrameGroups.size();
  }
}

// -----------------------------------------------------------------------------
int SpriteAnimationIndexAssetSet::directionId(const QString &name) const {
  int base = mParent ? mParent->numDirections() : 0;

  int index = mDirections.indexOf(name);
  if (index == -1) {
    return mParent ? mParent->directionId(name) : -1;
  }

  return base + index;
}

// -----------------------------------------------------------------------------
int SpriteAnimationIndexAssetSet::animationId(const QString &name) const {
  int base = mParent ? mParent->numAnimations() : 0;

  int index = mAnimations.indexOf(name);
  if (index == -1) {
    return mParent ? mParent->animationId(name) : -1;
  }

  return base + index;
}

// -----------------------------------------------------------------------------
int SpriteAnimationIndexAssetSet::frameGroupId(const QString &name) const {
  int base = mParent ? mParent->numFrameGroups() : 0;

  int index = mFrameGroups.indexOf(name);
  if (index == -1) {
    return mParent ? mParent->frameGroupId(name) : -1;
  }

  return base + index;
}

// -----------------------------------------------------------------------------
QMap<QString, int> SpriteAnimationIndexAssetSet::directions() const {
  QMap<QString, int> result = mParent ? mParent->directions() : QMap<QString, int>();
  int base = mParent ? mParent->numDirections() : 0;

  for (const QString &id : mDirections) {
    result[id] = base++;
  }

  return result;
}

// -----------------------------------------------------------------------------
QMap<QString, int> SpriteAnimationIndexAssetSet::animations() const {
  QMap<QString, int> result = mParent ? mParent->animations() : QMap<QString, int>();
  int base = mParent ? mParent->numAnimations() : 0;

  for (const QString &id : mAnimations) {
    result[id] = base++;
  }

  return result;
}

// -----------------------------------------------------------------------------
QMap<QString, int> SpriteAnimationIndexAssetSet::frameGroups() const {
  QMap<QString, int> result = mParent ? mParent->frameGroups() : QMap<QString, int>();
  int base = mParent ? mParent->numFrameGroups() : 0;

  for (const QString &id : mFrameGroups) {
    result[id] = base++;
  }

  return result;
}

// -----------------------------------------------------------------------------
int SpriteAnimationIndexAssetSet::animationId(const QString &direction, const QString &name) const {
  int dir = directionId(direction);
  int anim = animationId(name);

  if (dir == -1 || anim == -1) {
    return -1;
  }

  return dir + anim * numDirections();
}

// -----------------------------------------------------------------------------
bool SpriteAnimationIndexAssetSet::containsGeneratedSymbol(const QString &id) {
  if (mGeneratedSymbols.contains(id)) {
    return true;
  }

  mGeneratedSymbols.push_back(id);
  return false;
}

// -----------------------------------------------------------------------------
QString SpriteAnimationIndexAssetSet::buildFmaCodeOnce() {
  if (mReturnedFmaCode) {
    return "";
  }

  mReturnedFmaCode = true;

  QStringList result;
  if (mParent) {
    result << mParent->buildFmaCodeOnce();
  }
  if (!mDirections.isEmpty()) {
    result << "module Animation::Direction";
    for (const QString &id : mDirections) {
      result << id + " = " + QString::number(directionId(id));
    }
    result << "end";
  }
  if (!mAnimations.isEmpty()) {
    result << "module Animation::Animation";
    for (const QString &id : mAnimations) {
      result << id + " = " + QString::number(animationId(id));
    }
    result << "end";
  }

  return result.join('\n');
}
