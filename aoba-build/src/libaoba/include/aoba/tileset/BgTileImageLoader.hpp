#pragma once

#include <QSize>
#include <QSharedPointer>
#include "../palette/Palette.hpp"
#include "Tileset.hpp"

class QImage;
typedef QSharedPointer<class QImage> QImagePtr;

namespace Aoba {
class PaletteAssetSet;

class BgTileImageLoader {
public:
  //! Constructor
  BgTileImageLoader(PaletteAssetSet *palette, const QSize &bgTileSize)
    : mBgTileSize(bgTileSize)
  {
    mNumColorsPerPalette = palette->maxNumColors();
    mHasTransparence = palette->hasTransparence();
    mNumSnesColors = palette->numSnesColors();
  }

  //! Constructor
  BgTileImageLoader(int numColors, bool transparence, const QSize &bgTileSize)
    : mBgTileSize(bgTileSize)
  {
    mNumColorsPerPalette = numColors;
    mHasTransparence = transparence;
    mNumSnesColors = numColors;
  }

  //! Loads bg tiles from an image
  bool add(const QImagePtr &tilesImage, const QImagePtr &paletteImage);

  //! Loads bg tiles from an image
  bool add(const QImagePtr &tilesImage);

  //! Returns the result
  inline const QVector<TilesetBgTile> &result() const { return mResult; }

  //! Returns the generated palette
  inline const SnesPalettePtr &generatedPalette() const { return mGeneratedPalette; }

  //! Sets the palette
  inline void setTargetPalette(const SnesPalettePtr &palette) { mGeneratedPalette = palette; }

private:
  //! Palette
  int mNumColorsPerPalette;

  //! Num SNES colors
  int mNumSnesColors;

  //! Has transparence
  bool mHasTransparence;

  //! The generated palette
  SnesPalettePtr mGeneratedPalette;

  //! The BG tile size
  QSize mBgTileSize;
  
  //! The result
  QVector<TilesetBgTile> mResult;
};

}