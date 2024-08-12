#pragma once

#include <QMap>
#include "ImageConfig.hpp"
#include <aoba/image/ImageAccessor.hpp>

namespace Aoba {

class Tileset;
class Tilemap;
class PaletteGroup;
class GeneratedSpriteAnimation;

class BaseImage : public Asset<BaseImageAssetSet> {
public:
  //! Constructor
  BaseImage(BaseImageAssetSet *set, const QString &id);

  //! Returns the asset name
  virtual QString assetSymbolName() const = 0;

  //! Returns the index of an variant
  virtual int variantIndex(const QString &) const { return -1; }
};

}