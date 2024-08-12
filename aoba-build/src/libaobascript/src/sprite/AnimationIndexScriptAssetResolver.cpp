#include <aoba/log/Log.hpp>
#include <aoba/sprite/AnimationIndexScriptAssetResolver.hpp>
#include <aoba/sprite/SpriteAnimationIndex.hpp>
#include <fma/core/Number.hpp>
#include <fma/core/SymbolReference.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <fma/symbol/SymbolReference.hpp>

using namespace Aoba;
using namespace FMA::symbol;
using namespace FMA::core;
using namespace FMA::interpret;

// ----------------------------------------------------------------------------
QVariant AnimationIndexScriptAssetResolver::resolve(const QString &id, const QString &type) {
  int result;
  if (type == "animation_id") {
    result = mType->animationId(id);
    if (result < 0) {
      Aoba::log::error("Invalid animation: " + id);
    }
    return result;
  } else if (type == "direction_id") {
    result = mType->directionId(id);
    if (result < 0) {
      Aoba::log::error("Invalid direction: " + id);
    }
    return result;
  } else {
    Aoba::log::error("Unsupported animation index type: " + type);
    return QVariant();
  }
}
