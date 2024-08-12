#include <QDebug>
#include <sft/brr/BrrSample.hpp>
#include <sft/brr/BrrSampleChunk.hpp>

using namespace Sft;

// ----------------------------------------------------------------------------
BrrSample::BrrSample() {
}

// ----------------------------------------------------------------------------
BrrSample::~BrrSample() {
  clear();
}

// ----------------------------------------------------------------------------
void BrrSample::clear() {
  for (auto *block : mStream) {
    delete block;
  }

  mStream.clear();
  mLoopEnabled = false;
}

// ----------------------------------------------------------------------------
BrrSampleChunk *BrrSample::appendChunk() {
  mStream.push_back(new BrrSampleChunk());
  return mStream.last();
}

// ----------------------------------------------------------------------------
void BrrSample::writeTo(void *_data) const {
  char *data = (char*)_data;
  uint32_t counter = mStream.count() - 1;

  for (const auto &chunk : mStream) {
    chunk->writeTo(data, counter-- == 0, mLoopEnabled);
    data += NUM_BYTES_PER_BRR_SAMPLE_CHUNK;
  }
}
