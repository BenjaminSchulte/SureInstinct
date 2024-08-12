#pragma once

#include <QSharedPointer>
#include "ImageAccessor.hpp"

namespace Aoba {
class PaletteImage;

class PaletteImageAccessor : public ImageAccessor {
public:
  PaletteImageAccessor(const PaletteImagePtr &image) : mImage(image) {}

  ImageAccessorPtr createNewImage(int width, int height) const override;

  int width() const override;
  int height() const override;
  uint8_t bytesPerPixel() const override;
  QColor pixelColor(int x, int y) const override;
  void clearPixel(const QRect &) override;

  uint8_t *data() override;
  const uint8_t *constData() const override;

  bool isPaletteImage() const override { return true; }
  PaletteImagePtr toPaletteImage() const override { return mImage; }

  QImagePtr assignPalette(const SnesPaletteView &view) const;

  void dump() const override;

protected:
  PaletteImagePtr mImage;
};

}