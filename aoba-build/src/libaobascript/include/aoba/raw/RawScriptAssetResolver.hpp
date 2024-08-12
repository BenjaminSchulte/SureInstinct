#pragma once

#include "../script/ScriptAssetResolver.hpp"

namespace Aoba {
class RawAssetSet;

class RawScriptAssetResolver : public ScriptAssetResolver {
public:
  //! Constructor
  RawScriptAssetResolver(RawAssetSet *type) : mType(type) {}

  //! Returns a reference
  QVariant resolve(const QString &id, const QString &type) override;

protected:
  //! The level type
  RawAssetSet *mType;
};

}