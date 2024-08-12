#pragma once

#include <QSharedPointer>
#include "ImageAccessor.hpp"

namespace Aoba {
class RgbaImage;

class RgbaImageAccessor : public ImageAccessor {
public:
  //! Constructor
  RgbaImageAccessor(const RgbaImagePtr &image) : mImage(image) {}

  //! Creates a new image
  ImageAccessorPtr createNewImage(int width, int height) const override;

  //! Make pixel transparent
  void clearPixel(const QRect &) override;

  //! Returns the width
  int width() const override;

  //! Returns the height
  int height() const override;

  //! Returns the number of pixels per byte
  uint8_t bytesPerPixel() const override;

  //! Returns the color of a pixel
  QColor pixelColor(int x, int y) const override;

  //! Data accessor
  uint8_t *data() override;

  //! Data accessor
  const uint8_t *constData() const override;

  //! Returns true, because this is an RGBA image
  bool isRgbaImage() const override { return true; }

  //! Returns the image
  RgbaImagePtr toRgbaImage() const override { return mImage; }

protected:
  //! The image
  RgbaImagePtr mImage;
};

}