#include <aoba/tileset/BgTileImageLoader.hpp>
#include <aoba/palette/SnesPalette.hpp>
#include <aoba/palette/Palette.hpp>
#include <aoba/palette/PaletteGroup.hpp>
#include <aoba/image/QImageAccessor.hpp>
#include <aoba/image/PaletteImage.hpp>
#include <aoba/image/PaletteImageAccessor.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool BgTileImageLoader::add(const QImagePtr &tilesImage, const QImagePtr &paletteImage) {
  mGeneratedPalette = SnesPalette::fromImage(paletteImage.get());
  if (!mGeneratedPalette) {
    return false;
  }

  return add(tilesImage);
}

// -----------------------------------------------------------------------------
bool BgTileImageLoader::add(const QImagePtr &tilesImage) {
  QVector<ImageAccessorPtr> bgImages = QImageAccessor(tilesImage).split(mBgTileSize);
  int index = 0;
  int x = 0;
  int y = 0;
  
  for (auto &subImage : bgImages) {
    QVector<int> paletteIndices = subImage->findPalettes(mGeneratedPalette, mNumColorsPerPalette, mHasTransparence);
    if (paletteIndices.isEmpty()) {
      Aoba::log::error(QString("Could not find palette for subImage part %1 at %2,%3 (colors: %4)").arg(index).arg(x).arg(y).arg(mNumColorsPerPalette));
      Aoba::log::error("Required colors: " + colorsAsString(subImage->colors()).join(", "));
      for (int index=0; index<mGeneratedPalette->numSubPalettes(mNumColorsPerPalette); index++) {
        Aoba::log::error("Palette " + QString::number(index) + ": " + colorsAsString(mGeneratedPalette->subPalette(index, mNumColorsPerPalette, mHasTransparence).colors()).join(", "));
      }
      return false;
    }

    SnesPaletteView paletteView = mGeneratedPalette->subPalette(paletteIndices.first(), mNumColorsPerPalette, mHasTransparence);
    PaletteImagePtr paletteViewImage = subImage->convertToPaletteImage(paletteView)->toPaletteImage();
    paletteViewImage->overrideNumColors(mNumSnesColors);
    mResult << TilesetBgTile(paletteIndices.first(), paletteViewImage, index++);
    x += mBgTileSize.width();

    if (x >= tilesImage->width()) {
      x = 0;
      y += mBgTileSize.height();
    }
  }

  return true;
}
