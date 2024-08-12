#pragma once

#include <QImage>
#include <QSharedPointer>
#include "ImageAccessor.hpp"

namespace Aoba {

class QImageAccessor : public ImageAccessor {
public:
  QImageAccessor(const QImagePtr &image) : mImage(image) {}

  ImageAccessorPtr createNewImage(int width, int height) const override;

  static QImagePtr loadImage(const QString &filePath);

  int width() const override;
  int height() const override;
  uint8_t bytesPerPixel() const override;
  QColor pixelColor(int x, int y) const override;
  void clearPixel(const QRect &) override;

  ImageAccessorPtr slice(const QRect &rect) const override;

  uint8_t *data() override;
  const uint8_t *constData() const override;

  bool isQImage() const override { return true; }
  QImagePtr toQImage() const override { return mImage; }

protected:
  QImagePtr mImage;
};

inline QVector<QImagePtr> toQImageVector(const QVector<ImageAccessorPtr> &all) {
  QVector<QImagePtr> result;
  for (const auto &item : all) { result.push_back(item->toQImage()); }
  return result;
}

}