#pragma once

#include "../script/ScriptAssetResolver.hpp"

namespace Aoba {
class SpriteAssetSet;
class Sprite;

class SpriteScriptAssetResolver : public ScriptAssetResolver {
public:
  //! Constructor
  SpriteScriptAssetResolver(SpriteAssetSet *type) : mType(type) {}

  //! Returns a reference
  QVariant resolve(const QString &id, const QString &type) override;

  //! Prepares the resolve (only called for global assets)
  bool prepare(const QString &id, const QString &type) override;

protected:

  //! The Character type
  SpriteAssetSet *mType;
};

}