#include <aoba/compress/Lz77Compressor.hpp>

using namespace Aoba;

// ----------------------------------------------------------------------------
Lz77Compressor::Lz77Compressor(uint8_t previewSize, uint8_t dictionarySize)
  : mPreviewBufferSize((1 << previewSize) + 1)
  , mDictionaryBufferSize(1 << dictionarySize)
  , mMinLength(2)
  , mPreviewSize(0)
  , mDictionarySize(0)
  , mBufferSize(mPreviewBufferSize + mDictionaryBufferSize)
  , mCurrent(0)
  , mRight(0)
  , mNumSingleBytes(0)
  , mNumLookups(0)
{
  mBuffer = new char[mBufferSize];
}

// ----------------------------------------------------------------------------
Lz77Compressor::~Lz77Compressor() {
  delete[] mBuffer;
}

// ----------------------------------------------------------------------------
void Lz77Compressor::clear() {
  mStream.clear();
  mNumSingleBytes = 0;
  mNumLookups = 0;
}

// ----------------------------------------------------------------------------
int Lz77Compressor::match(int left, int right, int maxLength) {
  int length = 0;

  while (length < maxLength) {
    if (mBuffer[left] != mBuffer[right]) {
      break;
    }

    if (++left == mBufferSize) {
      left = 0;
    }

    if (++right == mBufferSize) {
      right = 0;
    }

    length++;
  }

  return length;
}

// ----------------------------------------------------------------------------
void Lz77Compressor::process() {
  char startCharacter = mBuffer[mCurrent];
  int occurence = -1;
  int length = 1;

  // Looks up occurences
  for (uint8_t i=0; i<mDictionarySize; i++) {
    int offset = mCurrent - i - 1;
    if (offset < 0) {
      offset += mBufferSize;
    }

    if (mBuffer[offset] != startCharacter) {
      continue;
    }

    int curLength = match(offset, mCurrent, std::min((int)mPreviewSize, mPreviewSize + i));
    if (curLength > length && curLength > mMinLength) {
      length = curLength;
      occurence = i;
    }
  }

  // Stores the result
  if (occurence >= 0) {
    mStream.push_back(Lz77CompressorPart(occurence, length - mMinLength));
    mNumLookups++;
  } else {
    mStream.push_back(Lz77CompressorPart(startCharacter));
    mNumSingleBytes++;
  }

  // Advances the pointers
  mCurrent = (mCurrent + length) % mBufferSize;
  mPreviewSize -= length;
  mDictionarySize += length;
  if (mDictionarySize > mDictionaryBufferSize) {
    mDictionarySize = mDictionaryBufferSize;
  }
} 

// ----------------------------------------------------------------------------
void Lz77Compressor::writeByte(char data) {
  mBuffer[mRight++] = data;
  mPreviewSize++;

  if (mRight == mBufferSize) {
    mRight = 0;
  }

  if (mPreviewSize == mPreviewBufferSize) {
    process();
  }
}

// ----------------------------------------------------------------------------
void Lz77Compressor::write(const char *data, uint32_t size) {
  while (size--) {
    writeByte(*data++);
  }
}

// ----------------------------------------------------------------------------
void Lz77Compressor::close() {
  while (mPreviewSize) {
    process();
  }
}

// ----------------------------------------------------------------------------
