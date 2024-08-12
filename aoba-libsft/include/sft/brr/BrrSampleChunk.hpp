#pragma once

#include <QString>
#include <QDebug>

namespace Sft {
  
//! Number of chunks 
const uint16_t NUM_SAMPLES_PER_CHUNK = 16;

//! Number of bytes per chunk
const uint8_t NUM_BYTES_PER_BRR_SAMPLE_CHUNK = 9;

//! Maximum shift for BRR samples
const uint8_t MAX_BRR_SAMPLE_SHIFT = 11;

//! Maximum value for a sample
const uint16_t BRR_SAMPLE_MAX = 0x7FFF;

struct BrrSampleChunk {
  //! Constructor
  BrrSampleChunk();

  //! Filter IDs
  enum Filter {
    FILTER_0 = 0,
    FILTER_1 = 1,
    FILTER_2 = 2,
    FILTER_3 = 3
  };

  //! Returns the header byte
  uint8_t headerByte(bool isLast=false, bool isLoop=false) const {
    return
      (isLast ? 0x01 : 0) |
      (isLoop ? 0x02 : 0) |
      (mFilter << 2) |
      (mShift << 4);
  }

  void clear();

  //! Returns the filter to use
  inline Filter filter() const { return mFilter; }

  //! Sets the filter to use
  void setFilter(Filter filter);

  //! Returns the shift to use
  inline uint8_t shift() const { return mShift; }

  //! Sets the shift to use
  void setShift(uint8_t shift);

  //! Returns the WAVE
  inline uint8_t *wave() { return mWave; }

  //! Returns the wave value signed
  uint16_t rawSample(uint8_t index) const {
    uint16_t value = mWave[index];
    if (value >= 0x8) {
      value |= 0xFFF0;
    }
    return value;
  }

  //! Returns the shifted sample
  inline int16_t shiftedSample(uint8_t index) const {
    return rawSample(index) << mShift;
  }

  //! Returns the length of the WAVE
  inline uint8_t length() const { return NUM_SAMPLES_PER_CHUNK; }

  //! Returns a nibble
  uint8_t nibble(uint8_t index) const {
    return mWave[index];
  }

  //! Writes the chunk to a address
  void writeTo(void *data, bool isLast=false, bool isLoop=false) const;
  
  //! Reads the sample from
  void readFrom(void *data);

protected:
  //! The filter
  Filter mFilter;

  //! The shift
  uint8_t mShift;

  //! The wave
  uint8_t mWave[NUM_SAMPLES_PER_CHUNK];
};

}