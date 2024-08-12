#include <aoba/log/Log.hpp>
#include <aoba/image/ImageCollection.hpp>

using namespace Aoba;

// ----------------------------------------------------------------------------
ImageCollection::ImageCollection(bool allowFlip)
  : mAllowFlip(allowFlip)
{
}

// ----------------------------------------------------------------------------
ImageCollection::~ImageCollection() {

}

// ----------------------------------------------------------------------------
void ImageCollection::clear() {
  mImagesInOrder.clear();
}

// ----------------------------------------------------------------------------
ImageCollectionPointer ImageCollection::get(const ImageAccessorPtr &image) {
  QString hash = image->hash();

  if (mImageDictionary.contains(hash)) {
    return mImageDictionary[hash];
  }

  return ImageCollectionPointer();
}

// ----------------------------------------------------------------------------
ImageCollectionPointer ImageCollection::add(const ImageAccessorPtr &image) {
  ImageCollectionPointer ptr(get(image));
  if (ptr.valid) {
    return ptr;
  }

  return addAlways(image);
}

// ----------------------------------------------------------------------------
ImageCollectionIndex ImageCollection::addRaw(const ImageAccessorPtr &image, bool isProtected) {
  ImageCollectionIndex index = mImagesInOrder.size();

  if (!isProtected) {
    if (mAllowFlip) {
      mImageDictionary.insert(image->flip(true,  true )->hash(), ImageCollectionPointer(index, true,  true ));
      mImageDictionary.insert(image->flip(false, true )->hash(), ImageCollectionPointer(index, false, true ));
      mImageDictionary.insert(image->flip(true,  false)->hash(), ImageCollectionPointer(index, true,  false));
    }
    mImageDictionary.insert(image->hash(), ImageCollectionPointer(index, false, false));
  }
  mImagesInOrder.push_back(image);

  return index;
}

// ----------------------------------------------------------------------------
ImageCollectionPointer ImageCollection::addAlways(const ImageAccessorPtr &image, bool isProtected) {
  return ImageCollectionPointer(addRaw(image, isProtected), false, false);
}