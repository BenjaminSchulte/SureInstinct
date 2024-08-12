#pragma once

#include <QDir>
#include "../asset/Asset.hpp"
#include "../asset/LinkedAssetSet.hpp"

namespace Aoba {
class Sprite;
class PaletteGroupSet;
class AssetByteList;

class SpriteAssetSet : public LinkedAssetSet<Sprite> {
public:
  //! Constructor
  SpriteAssetSet(Project *project, const QString &typeId, PaletteGroupSet *palettes, const QString &name, uint8_t smallSize, uint8_t largeSize, uint8_t frameBank, uint8_t animationBank, uint8_t paletteOffset=0);

  //! Returns the asset name
  inline const QString &assetName() const { return mName; }

  //! Returns the palettes
  inline PaletteGroupSet *palettes() const { return mPalettes; }

  //! Returns the small size
  inline uint8_t smallSize() const { return mSmallSize; }

  //! Returns the large size
  inline uint8_t largeSize() const { return mLargeSize; }

  //! Returns the frame bank
  inline uint8_t frameBank() const { return mFrameBank; }

  //! Returns the animation bank
  inline uint8_t animationBank() const { return mAnimationBank; }

  //! Returns the palette offset
  inline uint8_t paletteOffset() const { return mPaletteOffset; }

  //! The record address index
  inline AssetByteList *recordAddresses() const { return mRecordAddresses; }

  //! The record bank index
  inline AssetByteList *recordBanks() const { return mRecordBanks; }

private:
  //! The name
  QString mName;

  //! The palette group
  PaletteGroupSet *mPalettes;

  //! The small size
  uint8_t mSmallSize;

  //! The large size
  uint8_t mLargeSize;

  //! The frame bank
  uint8_t mFrameBank;

  //! The frame bank
  uint8_t mAnimationBank;

  //! The palette offset
  uint8_t mPaletteOffset;

  //! The record address index
  AssetByteList *mRecordAddresses;

  //! The record bank index
  AssetByteList *mRecordBanks;
};

}