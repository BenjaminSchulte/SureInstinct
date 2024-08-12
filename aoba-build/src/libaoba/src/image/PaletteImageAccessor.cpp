#include <iostream>
#include <QColor>
#include <QImage>
#include <aoba/image/PaletteImage.hpp>
#include <aoba/image/QImageAccessor.hpp>
#include <aoba/image/PaletteImageAccessor.hpp>
#include <aoba/palette/SnesPalette.hpp>

using namespace Aoba;

// ----------------------------------------------------------------------------
ImageAccessorPtr PaletteImageAccessor::createNewImage(int width, int height) const {
  QByteArray array;
  array.resize(width * height * mImage->bytesPerPixel());

  PaletteImagePtr newImage(new PaletteImage(array, mImage->numColors(), width, height));
  return ImageAccessorPtr(new PaletteImageAccessor(newImage));
}

// ----------------------------------------------------------------------------
QColor PaletteImageAccessor::pixelColor(int x, int y) const {
  return QColor(
    getRaw(x, y),
    getRaw(x, y),
    getRaw(x, y),
    getRaw(x, y) == 0 ? 0 : 255
  );
}

// ----------------------------------------------------------------------------
void PaletteImageAccessor::clearPixel(const QRect &_r) {
  QRect r = validRect(_r);
  uint8_t *row = this->data() + r.top() * mImage->bytesPerRow();

  for (int y=r.top(); y<=r.bottom(); y++) {
    uint8_t *col = row + r.left();
    for (int x=r.left(); x<=r.right(); x++) {
      *col++ = 0;
    }

    row += mImage->bytesPerRow();
  }
}

// ----------------------------------------------------------------------------
int PaletteImageAccessor::height() const {
  return mImage->height();
}

// ----------------------------------------------------------------------------
int PaletteImageAccessor::width() const {
  return mImage->width();
}

// ----------------------------------------------------------------------------
uint8_t PaletteImageAccessor::bytesPerPixel() const {
  return mImage->bytesPerPixel();
}

// ----------------------------------------------------------------------------
uint8_t *PaletteImageAccessor::data() {
  return reinterpret_cast<uint8_t*>(mImage->data().data());
}

// ----------------------------------------------------------------------------
const uint8_t *PaletteImageAccessor::constData() const {
  return reinterpret_cast<const uint8_t*>(mImage->constData().data());
}

// ----------------------------------------------------------------------------
QImagePtr PaletteImageAccessor::assignPalette(const SnesPaletteView &view) const {
  QImagePtr image(new QImage(width(), height(), QImage::Format_RGBA8888));
  QImageAccessor data(image);
  
  for (int y=0; y<height(); y++) {
    for (int x=0; x<width(); x++) {
      uint8_t index = getRaw(x, y);
      data.setRaw(x, y, view.color(index).toRgba());
    }
  }

  return image;
}

// ----------------------------------------------------------------------------
void PaletteImageAccessor::dump() const {
  for (int y=0; y<height(); y++) {
    QString str;

    for (int x=0; x<width(); x++) {
      uint8_t color = getRaw(x, y);

      if (color == 0) {
        str += ' ';
      } else if (color < 10) {
        str += ('1' + (color - 1));
      } else if (color < 16) {
        str += ('A' + (color - 10));
      } else {
        str += '?';
      }
    }
    str += "\n";

    std::cout << str.toStdString();
  }
}

// ----------------------------------------------------------------------------

