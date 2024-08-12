#include <aoba/log/Log.hpp>
#include <aoba/stream/StreamScriptAssetResolver.hpp>
#include <aoba/stream/Stream.hpp>
#include <fma/core/Number.hpp>
#include <fma/core/SymbolReference.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <fma/symbol/SymbolReference.hpp>

using namespace Aoba;
using namespace FMA::symbol;
using namespace FMA::core;
using namespace FMA::interpret;

// ----------------------------------------------------------------------------
QVariant StreamScriptAssetResolver::resolve(const QString &id, const QString &type) {
  Stream *stream = mType->get(id);
  if (!stream) {
    Aoba::log::warn("Unable to find stream " + id);
    return QVariant();
  }

  if (type == "address") {
    return stream->symbolName();
  } else {
    Aoba::log::warn("Unsupported stream type: " + type);
    return QVariant();
  }
}
