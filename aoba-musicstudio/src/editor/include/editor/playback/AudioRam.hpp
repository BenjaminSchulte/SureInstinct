#pragma once

#include <stdint.h>

namespace Editor {

struct AudioRamBlock {
  //! The usage
  enum Usage {
    FREE,
    SAMPLE_INDEX,
    SAMPLE,
    ECHO_BUFFER
  };

  //! Constructor
  AudioRamBlock(AudioRamBlock *previous, AudioRamBlock *next, Usage usage, uint32_t offset, uint32_t size)
    : mPrevious(previous), mNext(next), mUsage(usage), mOffset(offset), mSize(size)
  {}

  //! Deconstructor
  ~AudioRamBlock() {
  }

  //! Splits this block into two blocks
  AudioRamBlock *split(Usage usage, uint32_t size);

  //! Joins with the next block
  void joinNext();

  //! Joins with the next block
  void joinPrevious();

  //! Returns the usage
  inline Usage usage() const { return mUsage; }

  //! Sets the block to be free
  inline void free() { mUsage = FREE; }

  //! Returns whether this area is free
  inline bool isFree() const { return mUsage == FREE; }

  //! Returns the offset
  inline uint32_t offset() const { return mOffset; }
  
  //! Returns the size
  inline uint32_t size() const { return mSize; }

  //! Returns the previous block
  inline AudioRamBlock *previous() const { return mPrevious; }

  //! Returns the next block
  inline AudioRamBlock *next() const { return mNext; }

protected:
  //! Returns the previous block
  AudioRamBlock *mPrevious;

  //! Returns the next block
  AudioRamBlock *mNext;

  //! The usage
  Usage mUsage;

  //! The offset
  uint32_t mOffset;

  //! The size
  uint32_t mSize;
};

class AudioRam {
public:
  //! Constructor
  AudioRam();

  //! Deconstructot
  ~AudioRam();

  //! Clears the RAM
  void clear();

  //! Pointer to the RAM
  inline void *ram() const { return mRAM; }

  //! Pointer to the RAM
  inline void *ram(uint32_t address) const { return (uint8_t*)mRAM + address; }

  //! Allocates a new data, will return nullptr if allocation is not possible
  AudioRamBlock *allocate(AudioRamBlock::Usage usage, uint32_t size);

  //! Allocates a new data, will return nullptr if allocation is not possible
  void freeSample(AudioRamBlock *block);

  //! Dump
  void dump();

protected:
  //! Deletes the current ram
  void destroyRamIndex();

  //! The RAM
  void *mRAM;

  //! Pointer to the first RAM block
  AudioRamBlock *mFirstBlock;
};

}