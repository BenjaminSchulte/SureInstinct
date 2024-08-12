#pragma once

#include <QDir>
#include "../asset/Asset.hpp"

namespace Aoba {
class SpriteType;
class SpriteAnimationIndexAssetSet;

class SpriteTypeAssetSet : public AssetSet<SpriteType> {
public:
  //! Constructor
  SpriteTypeAssetSet(Project *project, const QString &name, SpriteAnimationIndexAssetSet *animationIndex) 
    : AssetSet<SpriteType>(name, project)
    , mAnimationIndex(animationIndex)
  {}

  //! Returns the sprite animation index
  inline SpriteAnimationIndexAssetSet *animationIndex() const { return mAnimationIndex; }

private:
  //! The sprite animation index
  SpriteAnimationIndexAssetSet *mAnimationIndex;
};

class SpriteType : public Asset<SpriteTypeAssetSet> {
public:
  //! Constructor
  SpriteType(SpriteTypeAssetSet *set, const QString &id);

  //! Loads the asset
  bool reload() override {
    return false;
  }

  //! Builds the asset
  bool build() override {
    return false;
  }
};

}