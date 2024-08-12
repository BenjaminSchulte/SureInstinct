#include <aoba/log/Log.hpp>
#include <aoba/image/ImageMapSplitter.hpp>
#include <aoba/image/ImageMapSplitterState.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
ImageMapSplitter::ImageMapSplitter(const ImageAccessorPtr &image, int width, int height)
  : mImage(image)
  , mImageWidth(image->width())
  , mImageHeight(image->height())
  , mBlockWidth(width)
  , mBlockHeight(height)
{
  mNumPixels = image->countVisiblePixel();
}

// -----------------------------------------------------------------------------
ImageMapSplitter::~ImageMapSplitter() {
  for (ImageMapSplitterState *state : mStates) {
    delete state;
  }
}

// -----------------------------------------------------------------------------
QVector<ImageMapSplitterSection> ImageMapSplitter::split() {
  mStates.push_back(createInitialState());

  while (!mStates.isEmpty()) {
    ImageMapSplitterState *state = mStates.front();
    mStates.pop_front();
    if (state->isDone()) {
      QVector<ImageMapSplitterSection> result(state->sections());
      delete state;
      return result;
    }

    mStates.append(state->process());
    delete state;
  
    resort();
  }

  return QVector<ImageMapSplitterSection>();
}

// -----------------------------------------------------------------------------
void ImageMapSplitter::resort() {
  std::sort(mStates.begin(), mStates.end(), [](ImageMapSplitterState *left, ImageMapSplitterState *right){
    return left->score() < right->score();
  });
}

// -----------------------------------------------------------------------------
ImageMapSplitterState *ImageMapSplitter::createInitialState() const {
  return new ImageMapSplitterState(
    this,
    mImage,
    QVector<ImageMapSplitterSection>(),
    mNumPixels
  );
}
