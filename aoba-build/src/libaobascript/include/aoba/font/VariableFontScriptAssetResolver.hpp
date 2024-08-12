#pragma once

#include "../script/ScriptAssetResolver.hpp"

namespace Aoba {
class VariableFontAssetSet;

class VariableFontScriptAssetResolver : public ScriptAssetResolver {
public:
  //! Constructor
  VariableFontScriptAssetResolver(VariableFontAssetSet *type) : mType(type) {}

  //! Returns a reference
  QVariant resolve(const QString &id, const QString &type) override;

protected:
  //! The level type
  VariableFontAssetSet *mType;
};

}