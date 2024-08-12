#pragma once

#include <QSharedPointer>

namespace Aoba {

class PaletteImage {
public:
  PaletteImage(const QByteArray &data, uint16_t numColors, uint32_t width, uint32_t height)
    : mData(data), mNumColors(numColors), mWidth(width), mHeight(height)
  {}

  inline QByteArray &data() { return mData; }
  inline const QByteArray &constData() const { return mData; }
  inline uint16_t numColors() const { return mNumColors; }
  inline uint8_t bytesPerPixel() const { return 1; }
  inline uint32_t width() const { return mWidth; }
  inline uint32_t height() const { return mHeight; }

  inline void overrideNumColors(uint16_t colors) { mNumColors = colors; }
  
  inline unsigned int bytesPerRow() const { return mWidth * bytesPerPixel(); }
  inline const uint8_t* scanline(int y) const { return (const uint8_t*)(mData.constData() + y * bytesPerRow()); }
  inline uint8_t colorAt(int x, int y) const { return scanline(y)[x]; }

protected:
  QByteArray mData;
  uint16_t mNumColors;
  uint32_t mWidth;
  uint32_t mHeight;
};

typedef QSharedPointer<PaletteImage> PaletteImagePtr;

}