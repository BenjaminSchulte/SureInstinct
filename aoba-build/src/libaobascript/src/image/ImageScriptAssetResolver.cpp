#include <aoba/log/Log.hpp>
#include <aoba/image/ImageScriptAssetResolver.hpp>
#include <aoba/image/Image.hpp>
#include <fma/core/Number.hpp>
#include <fma/core/SymbolReference.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <fma/symbol/SymbolReference.hpp>

using namespace Aoba;
using namespace FMA::symbol;
using namespace FMA::core;
using namespace FMA::interpret;

// ----------------------------------------------------------------------------
QVariant ImageScriptAssetResolver::resolve(const QString &id, const QString &type) {
  QStringList parts = id.split('/');
  BaseImage *image = mType->get(parts[0]);
  if (!image) {
    Aoba::log::error("Unable to find image " + parts[0]);
    return QVariant();
  }

  if (type == "address") {
    return image->assetSymbolName();
  } else if (type == "variant_index" && parts.size() == 2) {
    int id = image->variantIndex(parts[1]);
    if (id == -1) {
      Aoba::log::error("Unable to find variant " + parts[1] + " for image " + parts[0]);
      return QVariant();
    }
    return id;
  } else {
    Aoba::log::error("Unsupported image type: " + type);
    return QVariant();
  }
}
