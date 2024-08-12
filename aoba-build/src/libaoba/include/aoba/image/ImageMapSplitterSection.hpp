#pragma once

#include "./ImageAccessor.hpp"

namespace Aoba {

struct ImageMapSplitterSection {
  //! Constructor
  ImageMapSplitterSection() {}

  //! Constructor
  ImageMapSplitterSection(const ImageAccessorPtr &image, int x, int y)
    : image(image)
    , x(x)
    , y(y)
  {}

  //! The image
  ImageAccessorPtr image;

  //! The relative X position
  int x;

  //! The relative Y position
  int y;
};

}