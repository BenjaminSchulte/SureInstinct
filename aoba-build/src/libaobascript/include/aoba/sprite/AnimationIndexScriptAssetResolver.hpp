#pragma once

#include "../script/ScriptAssetResolver.hpp"

namespace Aoba {
class SpriteAnimationIndexAssetSet;

class AnimationIndexScriptAssetResolver : public ScriptAssetResolver {
public:
  //! Constructor
  AnimationIndexScriptAssetResolver(SpriteAnimationIndexAssetSet *type) : mType(type) {}

  //! Returns a reference
  QVariant resolve(const QString &id, const QString &type) override;

protected:

  //! The AnimationIndex type
  SpriteAnimationIndexAssetSet *mType;
};

}