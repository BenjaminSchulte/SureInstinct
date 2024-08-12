#include <aoba/log/Log.hpp>
#include <fma/core/Number.hpp>
#include <fma/core/SymbolReference.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <aoba/sfx/SfxLibrary.hpp>
#include <aoba/sfx/SfxLibraryScriptAssetResolver.hpp>

using namespace Aoba;
using namespace FMA::symbol;
using namespace FMA::core;
using namespace FMA::interpret;

// ----------------------------------------------------------------------------
QVariant SfxLibraryScriptAssetResolver::resolve(const QString &id, const QString &type) {
  QStringList parts = id.split('/');
  QString sfxLibraryId = parts[0];
  QString otherId = parts.size() > 1 ? parts[1] : "";

  SfxLibrary *sfxLibrary = mType->get(sfxLibraryId);
  if (!sfxLibrary) {
    Aoba::log::error("Unable to find sfxLibrary " + id);
    return QVariant();
  }

  if (type == "address") {
    return sfxLibrary->symbolName();
  } else if (type == "sfx_id") {
    return resolveSfx(sfxLibrary, otherId, type);
  } else {
    Aoba::log::error("Unsupported sfxLibrary type: " + type);
    return QVariant();
  }
}

// ----------------------------------------------------------------------------
QVariant SfxLibraryScriptAssetResolver::resolveSfx(SfxLibrary *lib, const QString &sfxName, const QString &type) {
  int sfxId = lib->sfxId(sfxName);
  if (sfxId == -1) {
    Aoba::log::error("Unable to find sfx " + sfxName + " in library '" + lib->id() + "'");
    return QVariant();
  }

  if (type == "sfx_id") {
    return sfxId;
  } else {
    Aoba::log::error("Unsupported sfxLibrary type: " + type);
    return QVariant();
  }
}