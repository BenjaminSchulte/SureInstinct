#pragma once

#include "Color.hpp"

namespace Sft {

class ColorPalette {
public:
  //! Returns the number of colors
  static int numColors();

  //! Returns the color at an index
  static Color color(int index);
};

}