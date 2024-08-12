#include <QPainter>
#include <QColor>
#include <QFile>
#include <aoba/log/Log.hpp>
#include <aoba/palette/SnesPalette.hpp>
#include <aoba/image/QImageAccessor.hpp>

using namespace Aoba;

// ----------------------------------------------------------------------------
ImageAccessorPtr QImageAccessor::createNewImage(int width, int height) const {
  QImagePtr newImage(new QImage(width, height, mImage->format()));

  return ImageAccessorPtr(new QImageAccessor(newImage));
}

// ----------------------------------------------------------------------------
QImagePtr QImageAccessor::loadImage(const QString &filePath) {
  QImagePtr image(new QImage());
  QIODevicePtr file(new QFile(filePath));
  if (!file->open(QIODevice::ReadOnly)) {
    Aoba::log::error("Unable to open image file: " + filePath);
    return QImagePtr();
  }

  if (!image->load(file.get(), "png")) {
    Aoba::log::error("Unable to load PNG image file for " + filePath);
    return QImagePtr();
  }

  return image;
}

// ----------------------------------------------------------------------------
ImageAccessorPtr QImageAccessor::slice(const QRect &rect) const {
  return ImageAccessorPtr(new QImageAccessor(QImagePtr(new QImage(mImage->copy(rect)))));
}

// ----------------------------------------------------------------------------
int QImageAccessor::height() const {
  return mImage->height();
}

// ----------------------------------------------------------------------------
int QImageAccessor::width() const {
  return mImage->width();
}

// ----------------------------------------------------------------------------
QColor QImageAccessor::pixelColor(int x, int y) const {
  return mImage->pixelColor(x, y);
}

// ----------------------------------------------------------------------------
void QImageAccessor::clearPixel(const QRect &r) {
  QPainter painter(mImage.get());
  painter.setCompositionMode(QPainter::CompositionMode_Source);
  painter.fillRect(r, Qt::transparent);
  painter.end();
}

// ----------------------------------------------------------------------------
uint8_t QImageAccessor::bytesPerPixel() const {
  return mImage->depth() / 8;
}

// ----------------------------------------------------------------------------
uint8_t *QImageAccessor::data() {
  return mImage->bits();
}

// ----------------------------------------------------------------------------
const uint8_t *QImageAccessor::constData() const {
  return mImage->constBits();
}

// ----------------------------------------------------------------------------

