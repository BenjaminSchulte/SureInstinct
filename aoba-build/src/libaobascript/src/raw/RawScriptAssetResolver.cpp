#include <aoba/log/Log.hpp>
#include <aoba/raw/RawScriptAssetResolver.hpp>
#include <aoba/raw/RawAsset.hpp>
#include <fma/core/Number.hpp>
#include <fma/core/SymbolReference.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <fma/symbol/SymbolReference.hpp>

using namespace Aoba;
using namespace FMA::symbol;
using namespace FMA::core;
using namespace FMA::interpret;

// ----------------------------------------------------------------------------
QVariant RawScriptAssetResolver::resolve(const QString &id, const QString &type) {
  RawAsset *raw = mType->get(id);
  if (!raw) {
    Aoba::log::error("Unable to find raw asset " + id);
    return QVariant();
  }

  if (type == "address") {
    return QVariant(raw->symbolName());
  } else {
    Aoba::log::error("Unsupported sfx type:" + type);
    return QVariant();
  }
}
