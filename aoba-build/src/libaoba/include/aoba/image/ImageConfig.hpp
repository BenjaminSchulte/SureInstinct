#pragma once

#include "BaseImageConfig.hpp"

namespace Aoba {
class ImageAssetSet : public BaseImageAssetSet {
public:
  //! Constructor
  ImageAssetSet(Project *project, TilemapAssetSet *tilemap, uint8_t headerRomBank);

  //! Adds a variant
  inline void addVariant(const QString &name) { mVariants.push_back(name); }

  //! Returns all variants
  QStringList variants() const override { return mVariants; }

  //! Returns the tilemap
  inline TilemapAssetSet *tilemap() const { return mTilemap; }

  //! Returns the tileset
  TilesetAssetSet *tileset() const;

private:
  //! List of all variants
  QStringList mVariants;

  //! The tilemap
  TilemapAssetSet *mTilemap;
};

}