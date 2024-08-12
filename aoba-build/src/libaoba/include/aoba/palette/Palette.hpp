#pragma once

#include <QString>
#include <QSharedPointer>
#include "../asset/Asset.hpp"
#include "../asset/LinkedAssetSet.hpp"

namespace Aoba {
class Palette;
class SnesPalette;

class PaletteAssetSet : public LinkedAssetSet<Palette> {
public:
  //! Constructor
  PaletteAssetSet(Project *project, uint16_t numColors, uint16_t maxColors, uint8_t romDataBank, uint8_t colorOffset, bool transparent, QString compression)
    : LinkedAssetSet<Palette>("palette", project)
    , mNumColors(numColors)
    , mMaxColors(maxColors)
    , mRomDataBank(romDataBank)
    , mColorOffset(colorOffset)
    , mTransparent(transparent)
    , mCompression(compression)
  {}

  //! Returns the total count
  inline int totalCount() const { return count(); }

  //! Returns the number of colors
  inline uint16_t numSnesColors() const { return mNumColors; }

  //! Returns the number of colors
  inline uint16_t maxNumColors() const { return mMaxColors; }

  //! Returns the color index
  inline uint8_t colorOffset() const { return mColorOffset; }

  //! Returns whether the first color is transparent
  inline bool hasTransparence() const { return mTransparent; }

  //! Creates a linker block
  inline FMA::linker::LinkerBlock *createLinkerBlock(const QString &id) {
    return assetLinkerObject().createLinkerBlock(id, mRomDataBank);
  }

  //! Returns a hash of the configuration
  QString hash() const;

  //! Returns a list of palettes which have already been built
  inline const QStringList &palettesBuilt() const { return mPalettesBuilt; }

  //! Add palette which has been built
  inline void addBuiltPalette(const QString &id) { mPalettesBuilt.push_back(id); }

  //! Returns the compression
  inline const QString &compression() const { return mCompression; }

protected:
  //! The number of colors
  uint16_t mNumColors;

  //! The number of colors
  uint16_t mMaxColors;

  //! The ROM data bank
  uint8_t mRomDataBank;

  //! The color offset
  uint8_t mColorOffset;

  //! Transparent
  bool mTransparent;

  //! Compression
  QString mCompression;
    
  //! List of palettes which have already been built
  QStringList mPalettesBuilt;
};

class Palette : public Asset<PaletteAssetSet> {
public:
  //! Constructor
  Palette(PaletteAssetSet *set, const QString &id, const QString &file, int row);

  //! Loads the asset
  bool reload() override;

  //! Builds the asset
  bool build() override;

  //! Returns the necessary FMA code
  QString getFmaCode() const override;

  //! Returns additional object files to include
  QStringList getFmaObjectFiles() const override;

  //! Returns the asset name
  QString assetSymbolName();

  //! Returns the palette
  inline const QSharedPointer<class SnesPalette> &palette() const { return mPalette; }

  //! Returns a hash
  QString hash() const;

  //! Returns the file
  inline const QString &file() const { return mFile; }

protected:
  //! The containing file
  QString mFile;

  //! The palette row
  int mRow;

  //! The SNES palette
  QSharedPointer<class SnesPalette> mPalette;

  //! The asset symbol name
  QString mAssetSymbolName;
};

}