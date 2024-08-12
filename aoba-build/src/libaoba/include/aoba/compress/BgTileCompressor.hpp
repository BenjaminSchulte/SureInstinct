#pragma once

class QBuffer;
class QByteArray;

namespace Aoba {

class BgTileCompressor {
public:
  BgTileCompressor(uint8_t *source, uint32_t size, QBuffer *target, int incSize);

  void prepare();
  void next();
  void finalize();

private:
  void initializeNextChunk();

  uint8_t *mSource;
  QBuffer *mTarget;
  int mIncrementSize;

  int mIndex;
  uint32_t mSize;

  uint8_t mLastValue;
  int mNewTimes;
  int mIncrementTimes;
};

}