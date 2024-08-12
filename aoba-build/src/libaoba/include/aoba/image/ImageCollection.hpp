#pragma once

#include <QVector>
#include <QMap>
#include "ImageAccessor.hpp"

namespace Aoba {

typedef uint32_t ImageCollectionIndex;

struct ImageCollectionPointer {
  ImageCollectionPointer() : valid(false) {}
  ImageCollectionPointer(ImageCollectionIndex index, bool x, bool y)
    : valid(true), index(index), mirrorX(x), mirrorY(y) {}

  bool valid;
  ImageCollectionIndex index;
  bool mirrorX;
  bool mirrorY;
};

class ImageCollection {
public:
  ImageCollection(bool allowFlip=true);
  virtual ~ImageCollection();

  void clear();

  ImageCollectionPointer get(const ImageAccessorPtr &);
  ImageCollectionPointer add(const ImageAccessorPtr &);
  ImageCollectionIndex addRaw(const ImageAccessorPtr &, bool isProtected=false);
  ImageCollectionPointer addAlways(const ImageAccessorPtr &, bool isProtected=false);

  inline const ImageAccessorPtr &getImage(int index) const { return mImagesInOrder[index]; }
  inline ImageAccessorPtr join() const { return mImagesInOrder.join(); }

  inline uint32_t numImages() const { return mImagesInOrder.size(); }

protected:
  bool mAllowFlip;
  QMap<QString, ImageCollectionPointer> mImageDictionary;
  ImageList mImagesInOrder;
};

}