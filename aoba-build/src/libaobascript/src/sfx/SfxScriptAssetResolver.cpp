#include <aoba/log/Log.hpp>
#include <aoba/sfx/SfxScriptAssetResolver.hpp>
#include <aoba/sfx/Sfx.hpp>
#include <fma/core/Number.hpp>
#include <fma/core/SymbolReference.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <fma/symbol/SymbolReference.hpp>

using namespace Aoba;
using namespace FMA::symbol;
using namespace FMA::core;
using namespace FMA::interpret;

// ----------------------------------------------------------------------------
QVariant SfxScriptAssetResolver::resolve(const QString &id, const QString &type) {
  Sfx *sfx = mType->get(id);
  if (!sfx) {
    Aoba::log::error("Unable to find sfx" + id);
    return QVariant();
  }

  if (type == "id") {
    Aoba::log::error("SFX IDs should be fetched via sfxlibrary");
    return QVariant();
  } else {
    Aoba::log::error("Unsupported sfx type:" + type);
    return QVariant();
  }
}
