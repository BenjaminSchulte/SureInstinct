#include <aoba/log/Log.hpp>
#include <aoba/font/VariableFontScriptAssetResolver.hpp>
#include <aoba/font/VariableFont.hpp>
#include <fma/core/Number.hpp>
#include <fma/core/SymbolReference.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <fma/symbol/SymbolReference.hpp>

using namespace Aoba;
using namespace FMA::symbol;
using namespace FMA::core;
using namespace FMA::interpret;

// ----------------------------------------------------------------------------
QVariant VariableFontScriptAssetResolver::resolve(const QString &id, const QString &type) {
  VariableFont *font = mType->get(id);
  if (!font) {
    Aoba::log::warn("Unable to find font " + id);
    return QVariant();
  }

  if (type == "address") {
    return font->fontIndexSymbolName();
  } else {
    Aoba::log::warn("Unsupported font type: " + type);
    return QVariant();
  }
}
