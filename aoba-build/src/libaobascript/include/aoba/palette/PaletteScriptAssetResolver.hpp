#pragma once

#include "../script/ScriptAssetResolver.hpp"

namespace Aoba {
class PaletteAssetSet;

class PaletteScriptAssetResolver : public ScriptAssetResolver {
public:
  //! Constructor
  PaletteScriptAssetResolver(PaletteAssetSet *type) : mType(type) {}

  //! Returns a reference
  QVariant resolve(const QString &id, const QString &type) override;

protected:
  //! The level type
  PaletteAssetSet *mType;
};

}