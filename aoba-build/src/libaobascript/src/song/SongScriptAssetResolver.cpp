#include <aoba/log/Log.hpp>
#include <aoba/song/SongScriptAssetResolver.hpp>
#include <aoba/song/Song.hpp>
#include <fma/core/Number.hpp>
#include <fma/core/SymbolReference.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <fma/symbol/SymbolReference.hpp>

using namespace Aoba;
using namespace FMA::symbol;
using namespace FMA::core;
using namespace FMA::interpret;

// ----------------------------------------------------------------------------
QVariant SongScriptAssetResolver::resolve(const QString &id, const QString &type) {
  Song *song = mType->get(id);
  if (!song) {
    Aoba::log::warn("Unable to find song " + id);
    return QVariant();
  }

  if (type == "id") {
    return mType->assetId(id);
  } else {
    Aoba::log::warn("Unsupported song type: " + type);
    return QVariant();
  }
}
