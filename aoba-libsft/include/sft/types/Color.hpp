#pragma once

#include <QString>

namespace Sft {

struct Color {
  //! Constructor
  Color() : mColor(0) {}

  //! Constructor
  explicit Color(uint32_t color) : mColor(color) {}

  //! Constructor
  Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a=0xFF)
    : mColor(b | g << 8 | r << 16 | a << 24)
  {}

  //! Returns the color value
  inline uint8_t red() const { return (mColor >> 16) & 0xFF; }

  //! Returns the color value
  inline uint8_t green() const { return (mColor >> 8) & 0xFF; }

  //! Returns the color value
  inline uint8_t blue() const { return (mColor >> 0) & 0xFF; }

  //! Returns the color value
  inline uint8_t alpha() const { return (mColor >> 24) & 0xFF; }

protected:
  //! The color value
  uint32_t mColor;
};

}