#pragma once

#include <QBuffer>
#include <QMap>
#include <aoba/compress/AbstractWriter.hpp>

namespace Aoba {

class LargeMapTilesWriter : public AbstractWriter {
public:
  LargeMapTilesWriter(Project *project, uint8_t bytesPerTile);

  bool write(const char *data, uint32_t size) override;

protected:
  void compressChunk(const char *data, uint16_t headerSize, uint16_t x, uint16_t y, uint16_t advanceX, uint16_t advanceY, uint16_t count, uint16_t width);

  QByteArray compressChunkData(const char *data, uint16_t x, uint16_t y, uint16_t advanceX, uint16_t advanceY, uint16_t count, uint16_t width);

  QBuffer mHeaderStream;
  QByteArray mDataBuffer;
  QBuffer mDataStream;
  uint8_t mBytesPerTile;

  QMap<QString, uint16_t> mGeneratedRows;
};

}