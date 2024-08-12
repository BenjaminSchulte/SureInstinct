#include <aoba/log/Log.hpp>
#include <QColor>
#include <QCryptographicHash>
#include <iostream>
#include <aoba/image/ImageAccessor.hpp>
#include <aoba/image/PaletteImage.hpp>
#include <aoba/image/PaletteImageAccessor.hpp>
#include <aoba/image/RgbaImage.hpp>
#include <aoba/image/RgbaImageAccessor.hpp>
#include <aoba/palette/SnesPalette.hpp>

using namespace Aoba;

// ----------------------------------------------------------------------------
QRect ImageAccessor::validRect(const QRect &rect) const {
  QPoint topLeft(
    std::max(0, rect.left()),
    std::max(0, rect.top())
  );

  if (topLeft.x() >= width() || topLeft.y() >= height()) {
    return QRect(0, 0, 0, 0);
  }

  return QRect(topLeft, QSize(
    std::min(rect.width(), width() - topLeft.x()),
    std::min(rect.height(), height() - topLeft.y())
  ));
}

// ----------------------------------------------------------------------------
ImageAccessorPtr ImageAccessor::slice(const QRect &_rect) const {
  QRect rect(validRect(_rect));
  QPoint target(0, 0);

  if (_rect.left() < 0) { target.setX(-_rect.left()); }
  if (_rect.top() < 0) { target.setY(-_rect.top()); }
  
  ImageAccessorPtr result(createNewImage(_rect.width(), _rect.height()));
  result->clearPixel(QRect(0, 0, _rect.width(), _rect.height()));
  result->draw(sharedFromThis(), rect, target);
  return result;
}

// ----------------------------------------------------------------------------
ImageColor ImageAccessor::getRaw(int x, int y) const {
  ImageColor color;
  const uint8_t *pointer = reinterpret_cast<const uint8_t*>(constData(x, y));

  switch (bytesPerPixel()) {
    case 1:
      color = *(reinterpret_cast<const uint8_t*>(pointer));
      break;

    case 2:
      color = *(reinterpret_cast<const uint16_t*>(pointer));
      break;

    case 3:
      color =
        *(reinterpret_cast<const uint16_t*>(pointer))
        | (*(reinterpret_cast<const uint8_t*>(pointer + 2)) << 16);
      break;

    default:
    case 4:
      color = *(reinterpret_cast<const uint32_t*>(pointer));
      break;
  }
  
  return color;
}

// ----------------------------------------------------------------------------
void ImageAccessor::setRaw(int x, int y, ImageColor color) {
  uint8_t *value = reinterpret_cast<uint8_t*>(data(x, y));

  switch (bytesPerPixel()) {
    case 1:
      *(reinterpret_cast<uint8_t*>(value)) = color;
      break;

    case 2:
      *(reinterpret_cast<uint16_t*>(value)) = color;
      break;

    case 3:
      *(reinterpret_cast<uint16_t*>(value)) = color;
      *(reinterpret_cast<uint8_t*>(value + 2)) = color >> 16;
      break;

    default:
    case 4:
      *(reinterpret_cast<uint32_t*>(value)) = color;
      break;
  }
}

// ----------------------------------------------------------------------------
void ImageAccessor::draw(const ConstImageAccessorPtr &image, const QPoint &target) {
  draw(image, QRect(0, 0, image->width(), image->height()), target);
}

// ----------------------------------------------------------------------------
void ImageAccessor::draw(const ConstImageAccessorPtr &image, const QRect &source) {
  draw(image, source, QPoint(0, 0));
}

// ----------------------------------------------------------------------------
void ImageAccessor::draw(const ConstImageAccessorPtr &image, const QRect &source, const QPoint &targetPoint) {
  QRect target(validRect(QRect(
    targetPoint.x(),
    targetPoint.y(),
    source.width(),
    source.height()
  )));

  int left = target.left();
  int top = target.top();
  int fromLeft = source.left();
  int fromTop = source.top();
  int height = target.height();
  int width = target.width();
  for (int y=0; y<height; y++) {
    for (int x=0; x<width; x++) {
      setRaw(x + left, y + top, image->getRaw(x + fromLeft, y + fromTop));
    }
  }
}

// ----------------------------------------------------------------------------
ImageList ImageAccessor::split(const QSize &chunkSize) const {
  int w = width();
  int h = height();

  ImageList result;
  for (int y=0; y<h; y+=chunkSize.height()) {
    for (int x=0; x<w; x+=chunkSize.width()) {
      result.push_back(slice(QRect(x, y, chunkSize.width(), chunkSize.height())));
    }
  }

  return result;
}

// ----------------------------------------------------------------------------
QVector<int> ImageAccessor::findPalettes(const SnesPalettePtr &palettes, uint16_t numColorsPerPalette, bool transparent) {
  QVector<int> indices;

  for (int i=palettes->numSubPalettes(numColorsPerPalette) - 1; i>=0; i--) {
    if (matchesPalette(palettes->subPalette(i, numColorsPerPalette, transparent))) {
      indices.push_back(i);
    }
  }

  return indices;
}

// ----------------------------------------------------------------------------
bool ImageAccessor::matchesPalette(const SnesPaletteView &view) {
  for (int y=height()-1; y>=0; y--) {
    for (int x=width()-1; x>=0; x--) {
      if (!view.contains(pixelColor(x, y))) {
        return false;
      }
    }
  }

  return true;
}

// ----------------------------------------------------------------------------
ImageAccessorPtr ImageAccessor::convertToPaletteImage(const SnesPaletteView &view) const {
  int w = width();
  int h = height();
  int index = 0;

  QByteArray result;
  result.resize(w * h);
  for (int y=0; y<h; y++) {
    for (int x=0; x<w; x++) {
      int color = view.indexOf(pixelColor(x, y));
      if (color < 0) {
        return ImageAccessorPtr();
      }

      result[index++] = color;
    }
  }

  return ImageAccessorPtr(new PaletteImageAccessor(
    PaletteImagePtr(new PaletteImage(result, view.numColors(), w, h))
  ));
}

// ----------------------------------------------------------------------------
ImageAccessorPtr ImageAccessor::convertToRgbaImage(const SnesPaletteView &view) const {
  int w = width();
  int h = height();
  int index = 0;

  QByteArray result;
  result.resize(w * h * 4);

  for (int y=0; y<h; y++) {
    for (int x=0; x<w; x++) {
      uint8_t colorIndex = *reinterpret_cast<const uint8_t*>(constData(x, y));
      const SnesColor &color = view.color(colorIndex);

      result[index++] = color.r;
      result[index++] = color.g;
      result[index++] = color.b;
      result[index++] = color.a;
    }
  }

  return ImageAccessorPtr(new RgbaImageAccessor(
    RgbaImagePtr(new RgbaImage(result, 4, w, h))
  ));
}

// ----------------------------------------------------------------------------
ImageAccessorPtr ImageAccessor::flip(bool flipX, bool flipY) const {
  int w = width();
  int h = height();

  ImageAccessorPtr newImage(createNewImage(w, h));
  for (int y=0; y<h; y++) {
    for (int x=0; x<w; x++) {
      newImage->setRaw(flipX ? w - x - 1 : x, flipY ? h - y - 1 : y, getRaw(x, y));
    }
  }
  return newImage;
}

// ----------------------------------------------------------------------------
QString ImageAccessor::hash() const {
  QCryptographicHash result(QCryptographicHash::Sha256);
  result.addData((const char*)constData(), width() * height() * bytesPerPixel());
  return result.result().toHex();
}

// ----------------------------------------------------------------------------
void ImageAccessor::dump() const {
  for (int y=0; y<height(); y++) {
    QString str;

    for (int x=0; x<width(); x++) {
      QColor color = pixelColor(x, y);
      
      if (color.alpha() < 128) {
        str += "\x1b[30;1m.";
      } else if (color.toHsl().saturationF() < 0.3) {
        str += " ";
      } else {
        int hue = color.toHsl().hue();

        if (hue < 30) { str += "\x1b[31"; }
        else if (hue < 90) { str += "\x1b[33"; }
        else if (hue < 150) { str += "\x1b[32"; }
        else if (hue < 210) { str += "\x1b[36"; }
        else if (hue < 270) { str += "\x1b[34"; }
        else if (hue < 330) { str += "\x1b[35"; }
        else { str += "\x1b[35"; }

        if (color.toHsl().lightnessF() > 0.5) {
          str += ";1";
        }

        str += "m#";
      }
    }
    str += "\x1b[m\n";
    std::cout << str.toStdString();
  }
}

// ----------------------------------------------------------------------------
QVector<QColor> ImageAccessor::colors() const {
  QVector<QColor> result;
  for (int y=0; y<height(); y++) {
    for (int x=0; x<width(); x++) {
      QColor color = pixelColor(x, y);

      if (color.alpha() == 0) {
        color = QColor(0,0,0,0);
      }

      if (!result.contains(color)) {
        result.push_back(color);
      }
    }
  }
  return result;
}

// ----------------------------------------------------------------------------
ImageAccessorPtr ImageAccessor::clone() const {
  ImageAccessorPtr copy = createNewImage(width(), height());
  copy->draw(sharedFromThis(), QPoint(0, 0));
  return copy;
}

// ----------------------------------------------------------------------------
ImageAccessorPtr ImageAccessor::resizeTo(int w, int h) const {
  ImageAccessorPtr copy = createNewImage(w, h);
  copy->clearPixel(QRect(0, 0, w, h));
  copy->draw(sharedFromThis(), QPoint(0, 0));
  return copy;
}

// ----------------------------------------------------------------------------
bool ImageAccessor::pixelIsTransparent(int x, int y) const {
  return pixelColor(x, y).alpha() < 128;
}

// ----------------------------------------------------------------------------
int ImageAccessor::countVisiblePixel() const {
  return countVisiblePixel(QRect(0, 0, width(), height()));
}

// ----------------------------------------------------------------------------
int ImageAccessor::countVisiblePixel(const QRect &_r) const {
  QRect r = validRect(_r);

  int num = 0;
  for (int y=r.top(); y<=r.bottom(); y++) {
    for (int x=r.left(); x<=r.right(); x++) {
      if (!pixelIsTransparent(x, y)) {
        num++;
      }
    }
  }
  return num;
}

// ----------------------------------------------------------------------------
QStringList Aoba::colorsAsString(const QVector<QColor> &colors) {
  QStringList result;
  for (const auto &color : colors) {
    result << QString("%1:%2:%3:%4").arg(color.red()).arg(color.green()).arg(color.blue()).arg(color.alpha());
  }
  return result;
}

// ----------------------------------------------------------------------------
QStringList Aoba::colorsAsString(const QVector<SnesColor> &colors) {
  QStringList result;
  for (const auto &color : colors) {
    result << QString("%1:%2:%3:%4").arg((int)color.r).arg((int)color.g).arg((int)color.b).arg((int)color.a);
  }
  return result;
}