#pragma once

#include "../script/ScriptAssetResolver.hpp"

namespace Aoba {
class SfxLibraryAssetSet;
class SfxLibrary;

class SfxLibraryScriptAssetResolver : public ScriptAssetResolver {
public:
  //! Constructor
  SfxLibraryScriptAssetResolver(SfxLibraryAssetSet *type) : mType(type) {}

  //! Returns a reference
  QVariant resolve(const QString &id, const QString &type) override;

protected:
  //! Returns a reference
  QVariant resolveSfx(SfxLibrary *lib, const QString &id, const QString &type);

  //! The SfxLibrary type
  SfxLibraryAssetSet *mType;
};

}