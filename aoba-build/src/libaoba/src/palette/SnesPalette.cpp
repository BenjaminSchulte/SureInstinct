#include <aoba/log/Log.hpp>
#include <QImage>
#include <stdlib.h>
#include <aoba/palette/SnesPalette.hpp>

using namespace Aoba;

// ----------------------------------------------------------------------------
SnesPalette::SnesPalette()
  : mColors(nullptr)
  , mNumColors(0)
{
}

// ----------------------------------------------------------------------------
SnesPalette::SnesPalette(uint16_t numColors)
  : mNumColors(numColors)
{
  mColors = (SnesColor*)malloc(sizeof(SnesColor) * mNumColors);
}

// ----------------------------------------------------------------------------
SnesPalette::SnesPalette(const SnesPalette &other)
  : QEnableSharedFromThis<SnesPalette>(other)
  , mColors(nullptr)
  , mNumColors(other.numColors())
{
  if (mNumColors) {
    mColors = (SnesColor*)malloc(sizeof(SnesColor) * mNumColors);
    memcpy(mColors, other.rawColorArray(), sizeof(SnesColor) * mNumColors);
  }
}

// ----------------------------------------------------------------------------
SnesPalette::~SnesPalette() {
  free(mColors);
}

// ----------------------------------------------------------------------------
QString SnesPalette::sortedHash() const {
  QStringList result;
  for (int i=0; i<mNumColors; i++) {
    if (mColors[i].transparent()) {
      result.append("-");
    } else {
      result.append(QString::number(mColors[i].toSnesColor()));
    }
  }
  std::sort(result.begin(), result.end());
  return result.join(',');
}

// ----------------------------------------------------------------------------
QString SnesPalette::hash() const {
  QStringList result;
  for (int i=0; i<mNumColors; i++) {
    if (mColors[i].transparent()) {
      result.append("-");
    } else {
      result.append(QString::number(mColors[i].toSnesColor()));
    }
  }
  return result.join(',');
}

// ----------------------------------------------------------------------------
SnesPaletteView SnesPalette::view(bool transparent) {
  return SnesPaletteView(sharedFromThis(), 0, mNumColors, transparent);
}

// ----------------------------------------------------------------------------
SnesPaletteView SnesPalette::subPalette(uint16_t index, uint16_t numColorsPerPalette, bool transparent) {
  if (index >= numSubPalettes(numColorsPerPalette)) {
    return SnesPaletteView();
  }

  int startIndex = index * numColorsPerPalette;
  int endIndex = qMin<int>(mNumColors, startIndex + numColorsPerPalette);
  int numColors = endIndex - startIndex;
  return SnesPaletteView(sharedFromThis(), startIndex, numColors, transparent);
}

// ----------------------------------------------------------------------------
SnesPalettePtr SnesPalette::fromImage(QImage *image, int firstRow, int lastRow, int colorsPerRow) {
  int width = image->width();
  int height = image->height();

  if (colorsPerRow <= 0) {
    colorsPerRow = width;
  }
  
  if (lastRow == -1 || lastRow >= height) {
    lastRow = height - 1;
  }
  if (firstRow >= height) {
    firstRow = height - 1;
  }

  int rows = lastRow - firstRow + 1;

  SnesPalettePtr palette(new SnesPalette(colorsPerRow * rows));
  int toIndex = 0;
  int fromIndex = 0;

  int startAt = firstRow * colorsPerRow;
  int lastIs = (lastRow + 1) * colorsPerRow - 1;

  for (int y=0; y<height; y++) {
    for (int x=0; x<width; x++, fromIndex++) {
      if (fromIndex < startAt) { continue; }
      if (fromIndex > lastIs) { continue; }

      palette->color(toIndex++) = SnesColor(image->pixelColor(x, y));
    }
  }

  return palette;
}

// ----------------------------------------------------------------------------
SnesPalettePtr SnesPalette::fromFile(const QIODevicePtr &file, const char *type, int firstRow, int lastRow, int colorsPerRow) {
  QImage image;
  if (!image.load(file.get(), type)) {
    Aoba::log::error("Could not load palette file");
    return SnesPalettePtr();
  }

  return SnesPalette::fromImage(&image, firstRow, lastRow, colorsPerRow);
}

// ----------------------------------------------------------------------------
void SnesPalette::clear() {
  free(mColors);
  mColors = nullptr;
  mNumColors = 0;
}

// ----------------------------------------------------------------------------
void SnesPalette::setNumColors(uint16_t num) {
  if (num == 0) {
    clear();
    return;
  }

  if (mColors) {
    mColors = (SnesColor*)realloc(mColors, sizeof(SnesColor) * num);
  } else {
    mColors = (SnesColor*)malloc(sizeof(SnesColor) * num);
  }

  mNumColors = num;
}

// ----------------------------------------------------------------------------
SnesPaletteView::SnesPaletteView(const SnesPalettePtr &palette, uint16_t offset, uint16_t numColors, bool transparent)
  : mPalette(palette)
  , mOffset(offset)
  , mNumColors(numColors)
  , mFirstIsTransparent(transparent)
{
}

// ----------------------------------------------------------------------------
int SnesPaletteView::indexOf(const QColor &rgb) const {
  if (rgb.alpha() < 128 && mFirstIsTransparent) {
    return 0;
  }

  int firstIndex = mFirstIsTransparent ? 1 : 0;

  for (int i=mNumColors-1; i>=firstIndex; i--) {
    if (mPalette->color(mOffset + i) == rgb) {
      return i;
    }
  }

  return -1;
}

// ----------------------------------------------------------------------------
int SnesPaletteView::indexOf(const SnesColor &rgb) const {
  if (rgb.transparent() && mFirstIsTransparent) {
    return 0;
  }

  int firstIndex = mFirstIsTransparent ? 1 : 0;

  for (int i=mNumColors-1; i>=firstIndex; i--) {
    if (mPalette->color(mOffset + i) == rgb) {
      return i;
    }
  }

  return -1;
}

// ----------------------------------------------------------------------------
bool SnesPaletteView::contains(const SnesPaletteView &other) const {
  for (int i=0; i<other.mNumColors; i++) {
    if (indexOf(other.color(i)) == -1) {
      return false;
    }
  }

  return true;
}

// ----------------------------------------------------------------------------
QVector<SnesColor> SnesPaletteView::colors() const {
  QVector<SnesColor> result;
  for (int i=0; i<mNumColors; i++) {
    result.push_back(color(i));
  }
  return result;
}

// ----------------------------------------------------------------------------
