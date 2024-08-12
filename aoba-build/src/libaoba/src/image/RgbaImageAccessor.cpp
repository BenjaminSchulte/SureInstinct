#include <iostream>
#include <aoba/log/Log.hpp>
#include <QColor>
#include <aoba/image/RgbaImage.hpp>
#include <aoba/image/RgbaImageAccessor.hpp>

using namespace Aoba;

// ----------------------------------------------------------------------------
ImageAccessorPtr RgbaImageAccessor::createNewImage(int width, int height) const {
  QByteArray array;
  array.resize(width * height * mImage->bytesPerPixel());

  RgbaImagePtr newImage(new RgbaImage(array, mImage->bytesPerPixel(), width, height));
  return ImageAccessorPtr(new RgbaImageAccessor(newImage));
}

// ----------------------------------------------------------------------------
QColor RgbaImageAccessor::pixelColor(int x, int y) const {
  return QColor(
    getRaw(x, y),
    getRaw(x, y),
    getRaw(x, y)
  );
}

// ----------------------------------------------------------------------------
void RgbaImageAccessor::clearPixel(const QRect &) {
  Aoba::log::warn("RgbaImageAccessor::clearPixel not implemented");
}

// ----------------------------------------------------------------------------
int RgbaImageAccessor::height() const {
  return mImage->height();
}

// ----------------------------------------------------------------------------
int RgbaImageAccessor::width() const {
  return mImage->width();
}

// ----------------------------------------------------------------------------
uint8_t RgbaImageAccessor::bytesPerPixel() const {
  return mImage->bytesPerPixel();
}

// ----------------------------------------------------------------------------
uint8_t *RgbaImageAccessor::data() {
  return reinterpret_cast<uint8_t*>(mImage->data().data());
}

// ----------------------------------------------------------------------------
const uint8_t *RgbaImageAccessor::constData() const {
  return reinterpret_cast<const uint8_t*>(mImage->constData().data());
}

// ----------------------------------------------------------------------------

