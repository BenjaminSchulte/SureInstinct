#pragma once

#include <QByteArray>
#include <aoba/image/PaletteImage.hpp>

namespace Aoba {
class Project;

class SnesPaletteImage {
public:
  //! Constructor
  SnesPaletteImage(const Aoba::PaletteImagePtr &image);

  //! Compiles the image
  QByteArray compile(uint32_t tileWidth, uint32_t tileHeight, uint8_t colorOffset) const;

  //! Compiles the image
  QByteArray compileColorImage256(uint32_t tileWidth, uint32_t tileHeight, uint8_t colorOffset) const;

  //! Checks whether the image is valid
  bool validateImage() const;

  //! Returns the bytes per pixel
  uint8_t bitsPerPixel() const;

  //! Returns the aligned bytes per pixel
  uint8_t alignedBitsPerPixel() const;

  //! Returns the required memory
  unsigned int requiredMemory() const;

  //! Returns the image
  inline const Aoba::PaletteImagePtr &image() const { return mImage; }

private:
  //! The image
  PaletteImagePtr mImage;

  //! The amount of colors
  uint16_t mNumColors;

  //! The aligned
  uint16_t mAlignToColors;

  //! Whether to use align
  bool mAlign;
};

}