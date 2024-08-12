#pragma once

#include <QVector>
#include "BrrSampleChunk.hpp"
#include "../Configuration.hpp"

namespace Sft {

class BrrSample {
public:
  //! Constructor
  BrrSample();

  //! Deconstructor
  ~BrrSample();

  //! Clear
  void clear();

  //! Returns whether looping is enabled
  inline bool loopEnabled() const { return mLoopEnabled; }

  //! Sets whether looping is enabled
  inline void setLoopEnabled(bool enable=true) { mLoopEnabled = enable; }

  //! Appends a new chunk
  BrrSampleChunk *appendChunk();

  //! Returns the amount of chunks
  inline uint32_t numChunks() const { return mStream.length(); }

  //! The duration on the SNES with the default note
  inline double snesDuration() const { return numChunks() * (double)NUM_SAMPLES_PER_CHUNK / (double)DSP_FREQUENCY; } 

  //! Returns the stream length in bytes
  inline uint32_t streamLengthInBytes() const { return mStream.length() * NUM_BYTES_PER_BRR_SAMPLE_CHUNK; }

  //! Returns a chunk
  inline BrrSampleChunk *chunk(uint32_t index) { return mStream[index]; }

  //! Returns a chunk
  inline const BrrSampleChunk *chunk(uint32_t index) const { return mStream[index]; }

  //! Write the sample into a RAM
  void writeTo(void *data) const;

protected:
  //! Whether looping is enabled
  bool mLoopEnabled = true;

  //! A list of all chunks
  QVector<BrrSampleChunk*> mStream;
};

}