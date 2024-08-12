#include <QByteArray>
#include <QBuffer>
#include <aoba/compress/BgTileCompressor.hpp>

using namespace Aoba;

#define MAX_NEW_TIMES 15
#define MAX_INC_TIMES 15

// ----------------------------------------------------------------------------
BgTileCompressor::BgTileCompressor(uint8_t *source, uint32_t size, QBuffer *target, int incSize)
  : mSource(source)
  , mTarget(target)
  , mIncrementSize(incSize)
  , mIndex(0)
  , mSize(size)
  , mLastValue(0)
  , mNewTimes(0)
  , mIncrementTimes(0)
{
}

// ----------------------------------------------------------------------------
void BgTileCompressor::prepare() {
  initializeNextChunk();
}

// ----------------------------------------------------------------------------
void BgTileCompressor::initializeNextChunk() {
  int newTimes = 0;
  int incTimes = 0;
  uint8_t lastValue = mLastValue;

  for (uint32_t i=mIndex; i<mSize; i++) {
    uint8_t n0 = (lastValue + mIncrementSize) & 0xFF;
    uint8_t b0 = mSource[i];
    uint8_t newLastValue;

    if (n0 == b0) {
      newLastValue = n0;
      incTimes++;
    } else {
      if (incTimes == 1) {
        newTimes++;
        incTimes = 0;
      } else if (incTimes > 1) {
        break;
      }

      newLastValue = b0;
      newTimes++;
    }

    lastValue = newLastValue;
    
    if (newTimes >= MAX_NEW_TIMES || incTimes >= MAX_INC_TIMES) {
      break;
    }
  }

  mNewTimes = std::min(newTimes, MAX_NEW_TIMES);
  mIncrementTimes = std::min(incTimes, MAX_INC_TIMES);

  if (mNewTimes == 0 && mIncrementTimes == 0) {
    // end of data
    return;
  }

  mTarget->putChar(mNewTimes | (mIncrementTimes << 4));
}

// ----------------------------------------------------------------------------
void BgTileCompressor::next() {
  if (mNewTimes == 0 && mIncrementTimes == 0) {
    initializeNextChunk();
  }

  if (mNewTimes > 0) {
    mLastValue = mSource[mIndex];
    mTarget->putChar(mLastValue);
    mIndex++;
    mNewTimes--;
  } else if (mIncrementTimes > 0) {
    mLastValue += mIncrementSize;
    mIndex++;
    mIncrementTimes--;
  }
}

// ----------------------------------------------------------------------------
void BgTileCompressor::finalize() {
  // nop
}

// ----------------------------------------------------------------------------
