#include <aoba/log/Log.hpp>
#include <QBuffer>
#include <aoba/compress/BgTileWriter.hpp>
#include <aoba/compress/BgTileCompressor.hpp>
#include <aoba/project/Project.hpp>

using namespace Aoba;

// ----------------------------------------------------------------------------
BgTileWriter::BgTileWriter(Project *project)
  : AbstractWriter(project)
  , mLow(new QByteArray)
  , mLowBuffer(new QBuffer(mLow))
  , mHigh(new QByteArray)
  , mHighBuffer(new QBuffer(mHigh))
  , mSize(0)
{
  mLowBuffer->open(QIODevice::WriteOnly);
  mHighBuffer->open(QIODevice::WriteOnly);
}

// ----------------------------------------------------------------------------
BgTileWriter::~BgTileWriter() {
  delete mHighBuffer;
  delete mHigh;
  delete mLowBuffer;
  delete mLow;
}

// ----------------------------------------------------------------------------
bool BgTileWriter::write(const char *data, uint32_t size) {
  while (size >= 2) {
    writePair(data[0], data[1]);
    data += 2;
    size -= 2;
  }

  if (size) {
    Aoba::log::error("BgTileWriter expects size to be multiplication of two.");
    return false;
  }

  return true;
}

// ----------------------------------------------------------------------------
void BgTileWriter::writePair(char low, char high) {
  mLowBuffer->putChar(low);
  mHighBuffer->putChar(high);

  mSize++;
}

// ----------------------------------------------------------------------------
bool BgTileWriter::finalize() {
  QBuffer buffer(&mBuffer);
  buffer.open(QIODevice::WriteOnly);

  uint8_t *highData = (uint8_t*)mHigh->data();
  uint8_t *lowData = (uint8_t*)mLow->data();

  BgTileCompressor high(highData, mHigh->size(), &buffer, 0);
  BgTileCompressor low(lowData, mLow->size(), &buffer, 1);

  uint16_t size = mSize;
  if ((size & 0xFF) == 0) {
    size += 0x100;
  }
  buffer.putChar(size & 0xFF);
  buffer.putChar(size >> 8);

  low.prepare();
  high.prepare();

  for (uint32_t i=0; i<mSize; i++) {
    low.next();
    high.next();
  }

  low.finalize();
  high.finalize();

  return true;
}

// ----------------------------------------------------------------------------
