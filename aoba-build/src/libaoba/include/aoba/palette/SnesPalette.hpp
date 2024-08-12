#pragma once

#include <QSharedPointer>
#include "SnesColor.hpp"

class QImage;
class QIODevice;
typedef QSharedPointer<QIODevice> QIODevicePtr;

namespace Aoba {

struct SnesPaletteView;
typedef QSharedPointer<class SnesPalette> SnesPalettePtr;

class SnesPalette : public QEnableSharedFromThis<SnesPalette> {
public:
  SnesPalette();
  SnesPalette(uint16_t numColors);
  SnesPalette(const SnesPalette &other);
  virtual ~SnesPalette();

  inline SnesColor &color(uint16_t index) { return mColors[index]; }
  inline SnesColor *rawColorArray() const { return mColors; }
  inline uint16_t numColors() const { return mNumColors; }
  inline uint16_t numSubPalettes(uint16_t numColorsPerPalette) const { return (mNumColors + numColorsPerPalette - 1) / numColorsPerPalette; }
  
  void clear();
  void setNumColors(uint16_t num);
  SnesPaletteView view(bool transparent=true);
  SnesPaletteView subPalette(uint16_t index, uint16_t numColorsPerPalette, bool transparent);

  QString sortedHash() const;
  QString hash() const;

  static SnesPalettePtr fromImage(QImage *image, int firstRow=0, int lastRow=-1, int colorsPerRow=0);
  static SnesPalettePtr fromFile(const QIODevicePtr &file, const char *type="png", int firstRow=0, int lastRow=-1, int colorsPerRow=0);

protected:
  SnesColor *mColors;
  uint16_t mNumColors;
};

struct SnesPaletteView {
  SnesPaletteView() : mPalette(), mOffset(0), mNumColors(0) {}
  SnesPaletteView(const SnesPalettePtr &palette, uint16_t offset, uint16_t numColors, bool transparent);
  SnesPaletteView(const SnesPaletteView &other) : mPalette(other.mPalette), mOffset(other.mOffset), mNumColors(other.mNumColors), mFirstIsTransparent(other.mFirstIsTransparent) {}
  
  inline const SnesColor &color(uint16_t index) const { return mPalette->color(index + mOffset); }
  inline SnesColor &color(uint16_t index) { return mPalette->color(index + mOffset); }
  inline uint16_t numColors() const { return mNumColors; }
  QVector<SnesColor> colors() const;

  int indexOf(const QColor &rgb) const;
  int indexOf(const SnesColor &rgb) const;
  inline bool contains(const QColor &rgb) const { return indexOf(rgb) >= 0; }
  inline bool contains(const SnesColor &rgb) const { return indexOf(rgb) >= 0; }
  bool contains(const SnesPaletteView &other) const;

protected:
  SnesPalettePtr mPalette;
  uint16_t mOffset;
  uint16_t mNumColors;
  bool mFirstIsTransparent;
};

}
