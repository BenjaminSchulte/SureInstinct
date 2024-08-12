#include <QImage>
#include <aoba/image/ImageAccessor.hpp>
#include <aoba/image/QImageAccessor.hpp>

using namespace Aoba;

// ----------------------------------------------------------------------------
int ImageList::minWidth() const {
  int width = INT_MAX;
  for (const auto &item : *this) { width = std::min(item->width(), width); }
  return width;
}

// ----------------------------------------------------------------------------
int ImageList::maxWidth() const {
  int width = INT_MIN;
  for (const auto &item : *this) { width = std::max(item->width(), width); }
  return width;
}

// ----------------------------------------------------------------------------
int ImageList::minHeight() const {
  int height = INT_MAX;
  for (const auto &item : *this) { height = std::min(item->height(), height); }
  return height;
}

// ----------------------------------------------------------------------------
int ImageList::maxHeight() const {
  int height = INT_MIN;
  for (const auto &item : *this) { height = std::max(item->height(), height); }
  return height;
}

// ----------------------------------------------------------------------------
int ImageList::totalWidth() const {
  int width = 0;
  for (const auto &item : *this) { width += item->width(); }
  return width;
}

// ----------------------------------------------------------------------------
int ImageList::totalHeight() const {
  int height = 0;
  for (const auto &item : *this) { height += item->height(); }
  return height;
}

// ----------------------------------------------------------------------------
ImageAccessorPtr ImageList::join() const {
  if (size() == 0) {
    return ImageAccessorPtr(new QImageAccessor(QImagePtr(new QImage())));
  }

  ImageAccessorPtr firstImage(front());
  int width = maxWidth();
  int height = totalHeight();
  
  ImageAccessorPtr newImage(firstImage->createNewImage(width, height));
  int y = 0;
  for (const auto &item : *this) {
    newImage->draw(item, QPoint(0, y));
    if (item->width() < width) {
      newImage->clearPixel(QRect(item->width(), y, width - item->width(), item->height()));
    }

    y += item->height();
  }

  return newImage;
}