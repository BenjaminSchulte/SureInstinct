#pragma once

#include "../script/ScriptAssetResolver.hpp"

namespace Aoba {
class SfxAssetSet;

class SfxScriptAssetResolver : public ScriptAssetResolver {
public:
  //! Constructor
  SfxScriptAssetResolver(SfxAssetSet *type) : mType(type) {}

  //! Returns a reference
  QVariant resolve(const QString &id, const QString &type) override;

protected:
  //! The level type
  SfxAssetSet *mType;
};

}