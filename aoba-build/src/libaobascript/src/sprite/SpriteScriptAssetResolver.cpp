#include <aoba/log/Log.hpp>
#include <aoba/sprite/SpriteScriptAssetResolver.hpp>
#include <aoba/sprite/Sprite.hpp>
#include <aoba/sprite/GeneratedSpriteAnimation.hpp>
#include <aoba/sprite/SpriteType.hpp>
#include <aoba/sprite/SpriteAnimationIndex.hpp>
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
QVariant SpriteScriptAssetResolver::resolve(const QString &id, const QString &type) {
  QStringList parts = id.split('/');
  QString spriteId = parts[0];
  QString otherId = parts.size() > 1 ? parts[1] : "";

  Sprite *sprite = mType->get(spriteId);
  if (!sprite) {
    Aoba::log::error("Unable to find sprite " + spriteId);
    return QVariant();
  }

  if (type == "id") {
    return mType->assetId(spriteId);
  } else if (type == "palette_group_address") {
    return sprite->mainPaletteGroup()->assetSymbolName();
  } else if (type == "palette_index") {
    if (!sprite->hasPalette(otherId)) {
      Aoba::log::error("Sprite " + spriteId + " does not contain any palette with ID " + otherId);
      return QVariant();
    }
    return sprite->paletteId(otherId);
  } else if (type == "animation_id") {
    int index = sprite->generatedAnimation(otherId)->animationId();
    if (index < 0) {
      Aoba::log::error("Can not find animation " + otherId + " for sprite " + sprite->id());
    }
    return index;
  } else if (type == "direction_id") {
    int index = sprite->spriteType()->animationIndex()->directionId(otherId);
    if (index < 0) {
      Aoba::log::error("Can not find direction " + otherId + " for sprite " + sprite->id());
    }
    return index;
  } else {
    Aoba::log::error("Unsupported asset type " + type);
    return QVariant();
  }
}

// ----------------------------------------------------------------------------
bool SpriteScriptAssetResolver::prepare(const QString &id, const QString &type) {
  QStringList parts = id.split('/');
  QString spriteId = parts[0];
  QString otherId = parts.size() > 1 ? parts[1] : "";

  Sprite *sprite = mType->get(spriteId);
  if (!sprite) {
    Aoba::log::error("Unable to find sprite " + spriteId);
    return false;
  }

  if (type == "animation_id") {
    int index = sprite->generatedAnimation(otherId)->animationId();
    if (index < 0) {
      Aoba::log::error("Can not find animation " + otherId + " for sprite " + sprite->id());
      return false;
    }
    return true;
  } else {
    return true;
  }
}