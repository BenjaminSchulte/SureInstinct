#pragma once

#include "./ImageAccessor.hpp"
#include "./ImageMapSplitterSection.hpp"

namespace Aoba {
class ImageMapSplitterState;

class ImageMapSplitter {
public:
  //! Constructor
  ImageMapSplitter(const ImageAccessorPtr &image, int width, int height);

  //! Deconstructor
  ~ImageMapSplitter();

  //! Splits the images
  QVector<ImageMapSplitterSection> split();

  //! Returns the image width
  inline int imageWidth() const { return mImageWidth; }

  //! Returns the image height
  inline int imageHeight() const { return mImageHeight; }

  //! Returns the block width
  inline int blockWidth() const { return mBlockWidth; }

  //! Returns the block height
  inline int blockHeight() const { return mBlockHeight; }

  //! Returns the number of pixels
  inline int numPixels() const { return mNumPixels; }

private:
  //! Returns a new initial state
  ImageMapSplitterState *createInitialState() const;

  //! Resorts all states
  void resort();

  //! The image
  ImageAccessorPtr mImage;

  //! The image width
  int mImageWidth;

  //! The image height
  int mImageHeight;

  //! The block width
  int mBlockWidth;

  //! The block height
  int mBlockHeight;

  //! Amount of pixels
  int mNumPixels;

  //! List of all states
  QVector<ImageMapSplitterState*> mStates;
};

}