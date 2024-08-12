#pragma once

#include "../script/ScriptAssetResolver.hpp"

namespace Aoba {
class PaletteGroupSet;

class PaletteGroupScriptAssetResolver : public ScriptAssetResolver {
public:
  //! Constructor
  PaletteGroupScriptAssetResolver(PaletteGroupSet *type) : mType(type) {}

  //! Returns a reference
  QVariant resolve(const QString &id, const QString &type) override;

protected:
  //! The level type
  PaletteGroupSet *mType;
};

}