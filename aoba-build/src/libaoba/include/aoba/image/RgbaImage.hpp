#pragma once

#include <QSharedPointer>

namespace Aoba {

class RgbaImage {
public:
  //! Constructor
  RgbaImage(const QByteArray &data, uint8_t bytesPerPixel, uint32_t width, uint32_t height)
    : mData(data), mBytesPerPixel(bytesPerPixel), mWidth(width), mHeight(height)
  {}

  //! Returns the datay
  inline QByteArray &data() { return mData; }

  //! Returns const data
  inline const QByteArray &constData() const { return mData; }

  //! Returns the bytes per pixel
  inline uint8_t bytesPerPixel() const { return mBytesPerPixel; }

  //! Returns the width
  inline uint32_t width() const { return mWidth; }

  //! Returns the height
  inline uint32_t height() const { return mHeight; }
  
  //! Returns the bytes per row
  inline unsigned int bytesPerRow() const { return mWidth * bytesPerPixel(); }

  //! Returns a pointer to a scanline
  inline const uint8_t* scanline(int y) const { return (const uint8_t*)(mData.constData() + y * bytesPerRow()); }
  
protected:
  //! The data
  QByteArray mData;

  //! The bytes per pixel (3 or 4)
  uint8_t mBytesPerPixel;

  //! The width
  uint32_t mWidth;

  //! The height
  uint32_t mHeight;

};

}