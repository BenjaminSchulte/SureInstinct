#pragma once

#include <QColor>
#include <cmath>

namespace Aoba {

struct  __attribute__((packed, aligned(1))) SnesColor {
  SnesColor() : r(0), g(0), b(0), a(255) {}
  SnesColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a=255) : r(r), g(g), b(b), a(a) {}
  SnesColor(const QColor &color)
    : r(color.red())
    , g(color.green())
    , b(color.blue())
    , a(color.alpha())
  {
  }

  inline bool transparent() const { return a == 0; }

  inline bool operator==(const QRgb &rgb) const { return operator==(QColor(rgb)); }
  inline bool operator==(const SnesColor &rgb) const { return transparent()==rgb.transparent() && toSnesColor()==rgb.toSnesColor(); }
  inline bool operator==(const QColor &color) const {
    if (a == 0) { return color.alpha() == 0; }
    return color.red() == r && color.green() == g && color.blue() == b;
  }

  inline uint32_t toRgba() const { 
    return (r) | (g << 8) | (b << 16) | (a << 24);
  }

  inline QColor toQColor() const { 
    return QColor(r, g, b, a);
  }

  uint16_t toSnesColor() const {
    if (transparent()) {
      return 0;
    }

    return ((r >> 3) & 0x001F)
      | ((g << 2) & 0x03E0)
      | ((b << 7) & 0x7C00);
  }

  SnesColor gammaCorrected(double correction) const {
    double red = r / 255.0;
    double green = g / 255.0;
    double blue = b / 255.0;
    
    return SnesColor(
      std::pow(red, correction) * 255.0,
      std::pow(green, correction) * 255.0,
      std::pow(blue, correction) * 255.0,
      a
    );
  }

  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
};

}
