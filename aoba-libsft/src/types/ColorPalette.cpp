#include <sft/types/ColorPalette.hpp>

using namespace Sft;

namespace {
  Color colors[] = {
    Color(255, 127, 127),
    Color(127, 255, 127),
    Color(127, 127, 255),
    Color(255, 255, 127),
    Color(127, 255, 255),
    Color(255, 127, 255),
  };
}

// -----------------------------------------------------------------------------
int ColorPalette::numColors() {
  return sizeof(colors) / sizeof(Color);
}

// -----------------------------------------------------------------------------
Color ColorPalette::color(int index) {
  return colors[index];
}
