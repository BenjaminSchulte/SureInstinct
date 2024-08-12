#include <QCryptographicHash>
#include <aoba/log/Log.hpp>
#include <aoba/compress/Lz77Writer.hpp>
#include <aoba/compress/LargeMapTilesWriter.hpp>
#include <aoba/project/Project.hpp>

using namespace Aoba;

// ----------------------------------------------------------------------------
LargeMapTilesWriter::LargeMapTilesWriter(Project *project, uint8_t bytesPerTile)
  : AbstractWriter(project)
  , mHeaderStream(&mBuffer)
  , mDataStream(&mDataBuffer)
  , mBytesPerTile(bytesPerTile)
{
}

// ----------------------------------------------------------------------------
bool LargeMapTilesWriter::write(const char *data, uint32_t size) {
  if (!mHeaderStream.open(QIODevice::WriteOnly) || ! mDataStream.open(QIODevice::WriteOnly)) {
    Aoba::log::error("Unable to open output stream for writing.");
    return false;
  }  

  if (size < 2) {
    Aoba::log::error("Missing layer header required by large_tilemap compressor.");
    return false;
  }

  uint16_t width = *reinterpret_cast<const uint8_t*>(&data[0]) + 1;
  uint16_t height = *reinterpret_cast<const uint8_t*>(&data[1]) + 1;
  if (size != (uint32_t)(width * height * 2 + 2)) {
    Aoba::log::error("Data size for large_tilemap compressor is invalid.");
    return false;
  }

  uint16_t headerSize = width * 2 + height * 2;
  for (uint16_t x=0; x<width; x++) {
    compressChunk(data, headerSize, x, 0, 0, 1, height, width);
  }
  for (uint16_t y=0; y<height; y++) {
    compressChunk(data, headerSize, 0, y, 1, 0, width, width);
  }

  mHeaderStream.write(mDataBuffer);

  return true;
}

// ----------------------------------------------------------------------------
void LargeMapTilesWriter::compressChunk(const char *data, uint16_t headerSize, uint16_t x, uint16_t y, uint16_t advanceX, uint16_t advanceY, uint16_t count, uint16_t width) {
  uint16_t offset;
  QByteArray result = compressChunkData(data, x, y, advanceX, advanceY, count, width);
  QString hash = QCryptographicHash::hash(result, QCryptographicHash::Sha256);

  if (mGeneratedRows.contains(hash)) {
    offset = mGeneratedRows[hash];
  } else {
    offset = headerSize + mDataStream.size();
    mDataStream.write(result);
    mGeneratedRows.insert(hash, offset);
  }

  mHeaderStream.putChar(offset);
  mHeaderStream.putChar(offset >> 8);
}

// ----------------------------------------------------------------------------
QByteArray LargeMapTilesWriter::compressChunkData(const char *data, uint16_t x, uint16_t y, uint16_t advanceX, uint16_t advanceY, uint16_t count, uint16_t width) {
  QByteArray row;
  row.resize(count * mBytesPerTile);

  uint16_t target=0;
  for (uint16_t i=0; i<count; i++) {
    const uint8_t *tile = reinterpret_cast<const uint8_t*>(&data[2 + x * 2 + y * 2 * width]);
    for (uint8_t j=0; j<mBytesPerTile; j++) {
      row[target++] = tile[j];
    }

    x += advanceX;
    y += advanceY;
  }

  return Lz77Writer::compressed(mProject, row);
}
