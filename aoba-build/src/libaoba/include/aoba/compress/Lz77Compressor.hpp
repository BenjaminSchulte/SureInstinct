#pragma once

#include <QByteArray>
#include <QVector>

namespace Aoba {

struct Lz77CompressorPart {
  enum Type {
    INVALID,
    BYTE,
    LOOKUP
  };

  Lz77CompressorPart() : type(INVALID) {}
  explicit Lz77CompressorPart(char byte) : type(BYTE), byte(byte) {}
  Lz77CompressorPart(int offset, int length) : type(LOOKUP), offset(offset), length(length) {}

  inline bool isByte() const { return type == BYTE; }
  inline bool isLookup() const { return type == LOOKUP; }

  Type type;
  char byte;
  int offset;
  int length;
};

class Lz77Compressor {
public:
  typedef QVector<Lz77CompressorPart> Stream;

  Lz77Compressor(uint8_t previewBufferSize, uint8_t dictionaryBufferSize);
  ~Lz77Compressor();

  void clear();
  inline const Stream &stream() const { return mStream; }

  inline uint32_t numSingleBytes() const { return mNumSingleBytes; }
  inline uint32_t numLookUps() const { return mNumLookups; }

  void write(const char *data, uint32_t size);
  void writeByte(char data);
  void close();

private:
  int match(int left, int right, int maxLength);
  void process();

  char *mBuffer;

  uint8_t mPreviewBufferSize;
  uint8_t mDictionaryBufferSize;

  uint8_t mMinLength;

  uint8_t mPreviewSize;
  uint8_t mDictionarySize;

  uint16_t mBufferSize;
  uint8_t mCurrent;
  uint8_t mRight;

  uint32_t mNumSingleBytes;
  uint32_t mNumLookups;

  Stream mStream;
};

}