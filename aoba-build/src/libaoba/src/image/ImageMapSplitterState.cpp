#include <QDebug>
#include <aoba/log/Log.hpp>
#include <aoba/image/ImageMapSplitter.hpp>
#include <aoba/image/ImageMapSplitterState.hpp>

using namespace Aoba;

namespace {
  int nextId = 0;
}

// -----------------------------------------------------------------------------
ImageMapSplitterState::ImageMapSplitterState(const ImageMapSplitter *splitter, const ImageAccessorPtr &image, const QVector<ImageMapSplitterSection> &sections, int numPixels)
  : mId(nextId++)
  , mSplitter(splitter)
  , mImage(image)
  , mSections(sections)
  , mNumPixelsLeft(numPixels)
{
}

// -----------------------------------------------------------------------------
QVector<ImageMapSplitterState*> ImageMapSplitterState::process() {
  QVector<ImageMapSplitterState*> newStates;
  if (!mNumPixelsLeft) {
    Aoba::log::error("Invalid call to ImageMapSplitterState::process()");
    return newStates;
  }

  QPoint firstPixel = findFirstPixel();
  QVector<ImageMapSplitterSection> options = getPositionsForPixel(firstPixel);
  for (const ImageMapSplitterSection &option : options) {
    ImageAccessorPtr image = mImage->clone();
    QRect partRect(option.x, option.y, mSplitter->blockWidth(), mSplitter->blockHeight());
    QVector<ImageMapSplitterSection> sectionCopy = mSections;
    sectionCopy.push_back(ImageMapSplitterSection(mImage->slice(partRect), option.x, option.y));
    image->clearPixel(partRect);

    ImageMapSplitterState *state = new ImageMapSplitterState(mSplitter, image, sectionCopy, image->countVisiblePixel());
    newStates.push_back(state);
  }

  return newStates;
}

// -----------------------------------------------------------------------------
QVector<ImageMapSplitterSection> ImageMapSplitterState::getPositionsForPixel(const QPoint &origin) const {
  QVector<ImageMapSplitterSection> result;

  int left = qMax(0, origin.x() - mSplitter->blockWidth() + 1);
  int right = qMin(mSplitter->imageWidth() - mSplitter->blockWidth(), origin.x());
  int top = qMin(mSplitter->imageHeight() - mSplitter->blockHeight(), origin.y());
  int bottom = top + mSplitter->blockHeight() - 1;

  if (right < 0) {
    left -= right;
    right = 0;
  }

  result.push_back(ImageMapSplitterSection(ImageAccessorPtr(), right, top));
  bool lastEmpty = true;
  bool curEmpty = true;
  bool nextEmpty = !mImage->countVisiblePixel(QRect(QPoint(left, top), QPoint(left, bottom)));

  for (int x=left; x<=right; x++) {
    curEmpty = nextEmpty;
    nextEmpty = !mImage->countVisiblePixel(QRect(QPoint(x + 1, top), QPoint(x + 1, bottom)));

    if (!curEmpty && nextEmpty && left >= x && x != right) {
      result.push_back(ImageMapSplitterSection(ImageAccessorPtr(), x, top));
    }

    if (curEmpty) {
      lastEmpty = true;
      continue;
    }

    if (lastEmpty && x != right) {
      lastEmpty = false;
      result.push_back(ImageMapSplitterSection(ImageAccessorPtr(), x, top));
    }
  }

  return result;
}

// -----------------------------------------------------------------------------
QPoint ImageMapSplitterState::findFirstPixel() const {
  int height = mImage->height();
  int width = mImage->width();

  for (int y=0; y<height; y++) {
    for (int x=0; x<width; x++) {
      if (!mImage->pixelIsTransparent(x, y)) {
        return QPoint(x, y);
      }
    }
  }

  // Fatal error
  Aoba::log::fatal("Call to ImageMapSplitterState::findFirstPixel() failed");
  return QPoint();
}

// -----------------------------------------------------------------------------
int ImageMapSplitterState::score() const {
  
  unsigned int pixelPerSectionPossible = mSplitter->blockHeight() * mSplitter->blockWidth();
  unsigned int totalPixelRemoved = mSplitter->numPixels() - mNumPixelsLeft;
  unsigned int totalPixelPossibleYet = mSections.count() * pixelPerSectionPossible;

  double sectionRatioScore = (mSections.count() == 0 ? 0 : totalPixelRemoved / (double)totalPixelPossibleYet);
  double imageProcessScore = 1.0 - (mNumPixelsLeft / (double)mSplitter->numPixels());
  double numSectionScore = (1000 - mSections.count()) / 1000.0;

  double score = sectionRatioScore * imageProcessScore * numSectionScore;

  return (1.0 - score) * 100000.0;
/*
  return mSections.count() * mSplitter->numPixels()
    + mNumPixelsLeft
    + (mNumPixelsLeft ? mSplitter->numPixels() : 0);
    */
}
