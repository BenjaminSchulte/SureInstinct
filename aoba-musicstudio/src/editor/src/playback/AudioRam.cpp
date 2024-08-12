#include <QDebug>
#include <editor/playback/AudioRam.hpp>

using namespace Editor;

// ----------------------------------------------------------------------------
AudioRamBlock *AudioRamBlock::split(Usage usage, uint32_t size) {
  if (size > mSize) {
    return nullptr;
  } else if (size == mSize) {
    mUsage = usage;
    return this;
  }

  uint32_t rest = mSize - size;
  
  AudioRamBlock *next = new AudioRamBlock(this, mNext, mUsage, mOffset + size, rest);
  if (mNext) {
    mNext->mPrevious = next;
  }
  mNext = next;
  mSize = size;
  mUsage = usage;
  return this;
}

// ----------------------------------------------------------------------------
void AudioRamBlock::joinNext() {
  if (mNext->next() && mNext->next()->usage() == mUsage) {
    AudioRamBlock *oldNext = mNext->next();
    mSize += mNext->size() + mNext->next()->size();
    mNext = mNext->next()->next();
    if (mNext) {
      mNext->mPrevious = this;
    }
    delete oldNext;
  } else {
    mSize += mNext->size();
    mNext = mNext->next();
    if (mNext) {
      mNext->mPrevious = this;
    }
  }
}

// ----------------------------------------------------------------------------
void AudioRamBlock::joinPrevious() {
  if (mPrevious->previous() && mPrevious->previous()->usage() == mUsage) {
    AudioRamBlock *oldPrevious = mPrevious->previous();
    mSize += mPrevious->size() + mPrevious->previous()->size();
    mOffset = mPrevious->previous()->offset();
    mPrevious = mPrevious->previous()->previous();
    if (mPrevious) {
      mPrevious->mNext = this;
    }
    delete oldPrevious;
  } else {
    mSize += mPrevious->size();
    mOffset = mPrevious->offset();
    mPrevious = mPrevious->previous();
    if (mPrevious) {
      mPrevious->mNext = this;
    }
  }
}



// ----------------------------------------------------------------------------
AudioRam::AudioRam()
  : mRAM(malloc(64 * 1024))
  , mFirstBlock(nullptr)
{
  clear();
}

// ----------------------------------------------------------------------------
AudioRam::~AudioRam() {
  free(mRAM);
  destroyRamIndex();
}

// ----------------------------------------------------------------------------
void AudioRam::clear() {
  destroyRamIndex();

  mFirstBlock = new AudioRamBlock(nullptr, nullptr, AudioRamBlock::FREE, 0, 64 * 1024);
}

// ----------------------------------------------------------------------------
void AudioRam::destroyRamIndex() {
  AudioRamBlock *block;
  AudioRamBlock *next = mFirstBlock;
  while (next) {
    block = next;
    next = next->next();
    delete block;
  }

  mFirstBlock = nullptr;
}

// ----------------------------------------------------------------------------
void AudioRam::freeSample(AudioRamBlock *block) {
  if (mFirstBlock == block) {
    mFirstBlock = block->next();
  }

  if (block->previous() && block->previous()->isFree()) {
    block->previous()->joinNext();
  } else if (block->next() && block->next()->isFree()) {
    block->next()->joinPrevious();
  } else {
    block->free();
  }
}

// ----------------------------------------------------------------------------
void AudioRam::dump() {
  AudioRamBlock *cur = mFirstBlock;

  while (cur) {
    qDebug() << cur << cur->offset() << cur->size() << cur->isFree() << cur->previous();
    cur = cur->next();
  }
}

// ----------------------------------------------------------------------------
AudioRamBlock *AudioRam::allocate(AudioRamBlock::Usage usage, uint32_t size) {
  AudioRamBlock *bestBlock = nullptr;
  uint32_t bestOverhang = 0;

  AudioRamBlock *current = mFirstBlock;
  for (; current; current = current->next()) {
    // Tests whether the block is large enought
    if (!current->isFree() || current->size() < size) {
      continue;
    }

    // Tests the overhang
    uint32_t overhang = qAbs((int)current->size() - (int)size);
    if (bestBlock == nullptr || bestOverhang > overhang) {
      bestBlock = current;
      bestOverhang = overhang;
    }
  }

  if (bestBlock) {
    return bestBlock->split(usage, size);
  } else {
    return nullptr;
  }
}
