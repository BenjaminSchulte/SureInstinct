#pragma once

#include "BaseImageConfig.hpp"

namespace Aoba {
class PaletteGroupSet;

class RawImageAssetSet : public BaseImageAssetSet {
public:
  //! Constructor
  RawImageAssetSet(Project *project, uint8_t headerRomBank, PaletteGroupSet *palette, const QSize &tileSize, uint8_t indexMultiplier);

  //! Returns all variants
  QStringList variants() const override { return {"default"}; }

  //! Returns the tileset
  inline PaletteGroupSet *palette() const { return mPalette; }

  //! Returns the tiles
  inline const QSize &tileSize() const { return mTileSize; }

  //! Returns the tiles
  inline uint8_t indexMultiplier() const { return mIndexMultiplier; }

private:
  //! List of all variants
  QStringList mVariants;

  //! The tileset
  PaletteGroupSet *mPalette;

  //! The tiles ID
  QSize mTileSize;

  //! The index multiplier
  uint8_t mIndexMultiplier;
};

}