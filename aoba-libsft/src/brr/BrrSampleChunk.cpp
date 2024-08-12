#include <QDebug>
#include <sft/brr/BrrSampleChunk.hpp>

using namespace Sft;

// ----------------------------------------------------------------------------
BrrSampleChunk::BrrSampleChunk()
  : mFilter(FILTER_0)
  , mShift(0)
{
}

// ----------------------------------------------------------------------------
void BrrSampleChunk::clear() {
  for (int i=0; i<NUM_SAMPLES_PER_CHUNK; i++) {
    mWave[i] = 0;
  }
}

// ----------------------------------------------------------------------------
void BrrSampleChunk::setFilter(Filter filter) {
  if (filter < FILTER_0 || filter > FILTER_3) {
    return;
  }

  mFilter = filter;
}

// ----------------------------------------------------------------------------
void BrrSampleChunk::setShift(uint8_t shift) {
  if (shift > MAX_BRR_SAMPLE_SHIFT) {
    shift = MAX_BRR_SAMPLE_SHIFT;
  }

  mShift = shift;
}

// ----------------------------------------------------------------------------
void BrrSampleChunk::writeTo(void *_data, bool isLast, bool isLoop) const {
  uint8_t *data = (uint8_t*)_data;

  data[0] = headerByte(isLast, isLoop);
  for (uint8_t i=0; i<(NUM_SAMPLES_PER_CHUNK/2); i++) {
    data[1 + i] = (nibble(i * 2 + 0) << 4) | (nibble(i * 2 + 1));
  }
}

// ----------------------------------------------------------------------------
void BrrSampleChunk::readFrom(void *_data) {
  uint8_t *data = (uint8_t*)_data;

  mFilter = (Filter)((data[0] >> 2) & 3);
  mShift = data[0] >> 4;

  for (uint8_t i=0; i<(NUM_SAMPLES_PER_CHUNK/2); i++) {
    mWave[i * 2 + 0] = (data[i + 1] >> 4) & 0x0F;
    mWave[i * 2 + 1] = (data[i + 1]) & 0x0F;
  }
}