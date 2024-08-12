#pragma once

#include <QVector>
#include "./ImageAccessor.hpp"
#include "./ImageMapSplitterSection.hpp"

namespace Aoba {
class ImageMapSplitter;

class ImageMapSplitterState {
public:
  //! Constructor
  ImageMapSplitterState(const ImageMapSplitter *splitter, const ImageAccessorPtr &image, const QVector<ImageMapSplitterSection> &sections, int numPixels);

  //! Processes this state
  QVector<ImageMapSplitterState*> process();

  //! Whether the state is finished
  inline bool isDone() const { return mNumPixelsLeft == 0; }

  //! Returns the score
  int score() const;
  
  //! Returns the sections
  inline const QVector<ImageMapSplitterSection> sections() const { return mSections; }

private:
  //! Returns the first pixel
  QPoint findFirstPixel() const;

  //! Returns possible section positions
  QVector<ImageMapSplitterSection> getPositionsForPixel(const QPoint &) const;

  //! Internal ID (for debugging)
  int mId;

  //! The parent
  const ImageMapSplitter *mSplitter;

  //! The image
  ImageAccessorPtr mImage;

  //! The sections
  QVector<ImageMapSplitterSection> mSections;

  //! The number of pixels left
  int mNumPixelsLeft;
};

}