#pragma once

#include <QDir>
#include <QMap>
#include <QVector>
#include "../asset/Asset.hpp"
#include "../asset/LinkedAssetSet.hpp"

namespace Aoba {
class SpriteAnimationIndex;

class SpriteAnimationIndexAssetSet : public LinkedAssetSet<SpriteAnimationIndex> {
public:
  //! Constructor
  SpriteAnimationIndexAssetSet(Project *project, SpriteAnimationIndexAssetSet *parent, int animationScriptBank=-1, int frameScriptBank=-1)
    : LinkedAssetSet<SpriteAnimationIndex>("sprite_animation_index", project)
    , mParent(parent)
    , mAnimationScriptBank(animationScriptBank)
    , mFrameReferenceBank(frameScriptBank)
  {}

  //! Returns the root set
  inline SpriteAnimationIndexAssetSet *root() {
    if (mParent) {
      return mParent->root();
    } else {
      return this;
    }
  }

  //! Adds a direction
  inline void addDirection(const QString &name) { mDirections.push_back(name); }

  //! Adds an animation
  inline void addAnimation(const QString &name) { mAnimations.push_back(name); }

  //! Adds an animation
  inline void addFrameGroup(const QString &name) { mFrameGroups.push_back(name); }

  //! Returns the number of directions
  int numDirections() const;

  //! Returns the ID of a direction (or -1 if not found)
  int directionId(const QString &name) const;

  //! Returns whether a direction exists
  bool hasDirection(const QString &name) const { return directionId(name) != -1; }

  //! Returns all directions
  QMap<QString, int> directions() const;

  //! Returns the number of directions
  int numAnimations() const;

  //! Returns the ID of a animation (or -1 if not found)
  int animationId(const QString &name) const;

  //! Returns whether a direction exists
  bool hasAnimation(const QString &name) const { return animationId(name) != -1; }

  //! Returns all animations
  QMap<QString, int> animations() const;

  //! Returns the ID of a specific animation
  int animationId(const QString &direction, const QString &name) const;

  //! Returns the number of directions
  int numFrameGroups() const;

  //! Returns the ID of a animation (or -1 if not found)
  int frameGroupId(const QString &name) const;

  //! Returns whether a direction exists
  bool hasFrameGroup(const QString &name) const { return frameGroupId(name) != -1; }

  //! Returns all frameGroups
  QMap<QString, int> frameGroups() const;

  //! Returns whether a symbol hash been generated for this or not
  bool containsGeneratedSymbol(const QString &id);

  //! Returns the FMA code
  QString buildFmaCodeOnce();

  //! Returns the animation bank
  inline int animationScriptBank() const { return mAnimationScriptBank; }

  //! Returns the frame reference bank
  inline int frameReferenceBank() const { return mFrameReferenceBank; }

private:
  //! The parent asset set, might be nullptr
  SpriteAnimationIndexAssetSet *mParent;

  //! List of all directions
  QVector<QString> mDirections;

  //! List of all animations
  QVector<QString> mAnimations;

  //! List of all animations
  QVector<QString> mFrameGroups;

  //! List of all generated symbols
  QVector<QString> mGeneratedSymbols;

  //! Whether the FMA code has already been built
  bool mReturnedFmaCode = false;

  //! Bank to store animation scripts at
  int mAnimationScriptBank;

  //! Bank to store frame references at
  int mFrameReferenceBank;
};


class SpriteAnimationIndex : public Asset<SpriteAnimationIndexAssetSet> {
public:
  //! Constructor
  SpriteAnimationIndex(SpriteAnimationIndexAssetSet *set, const QString &id);
};

}