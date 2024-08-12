#include <aoba/log/Log.hpp>
#include <aoba/palette/PaletteGroupScriptAssetResolver.hpp>
#include <aoba/palette/PaletteGroup.hpp>
#include <fma/core/Number.hpp>
#include <fma/core/SymbolReference.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <fma/symbol/SymbolReference.hpp>

using namespace Aoba;
using namespace FMA::symbol;
using namespace FMA::core;
using namespace FMA::interpret;

// ----------------------------------------------------------------------------
QVariant PaletteGroupScriptAssetResolver::resolve(const QString &id, const QString &type) {
  PaletteGroup *palette = mType->get(id);
  if (!palette) {
    Aoba::log::error("Unable to find palette group: " + id);
    return QVariant();
  }

  if (type == "address") {
    return palette->assetSymbolName();
  } else {
    Aoba::log::warn("Unsupported palette group type: " + type);
    return QVariant();
  }
}
