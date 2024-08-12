#pragma once

#include "../script/ScriptAssetResolver.hpp"

namespace Aoba {
class BaseImageAssetSet;

class ImageScriptAssetResolver : public ScriptAssetResolver {
public:
  //! Constructor
  ImageScriptAssetResolver(BaseImageAssetSet *type) : mType(type) {}

  //! Returns a reference
  QVariant resolve(const QString &id, const QString &type) override;

protected:
  //! The level type
  BaseImageAssetSet *mType;
};

}