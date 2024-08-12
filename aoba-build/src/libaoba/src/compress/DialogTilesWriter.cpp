#include <aoba/log/Log.hpp>
#include <QBuffer>
#include <aoba/compress/DialogTilesWriter.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/tileset/BgTile.hpp>

using namespace Aoba;

namespace {
  const int MAX_REPEAT_TIMES = 0x1F;
}

// ----------------------------------------------------------------------------
DialogTilesWriter::DialogTilesWriter(Project *project)
  : AbstractWriter(project)
  , mStream(&mBuffer)
{
}

// ----------------------------------------------------------------------------
bool DialogTilesWriter::write(const char *data, uint32_t size) {
  if (!mStream.open(QIODevice::WriteOnly)) {
    Aoba::log::error("Unable to open output stream for writing.");
    return false;
  }

  if (size < 2) {
    Aoba::log::error("Missing layer header required by dialog_tiles compressor.");
    return false;
  }

  uint16_t width = *reinterpret_cast<const uint8_t*>(&data[0]) + 1;
  uint16_t height = *reinterpret_cast<const uint8_t*>(&data[1]) + 1;
  if (size != (uint32_t)(width * height * 2 + 2)) {
    Aoba::log::error("Data size for dialog_tiles compressor is invalid.");
    return false;
  }

  mStream.putChar(size >> 1);
  mStream.putChar(size >> 9);

  for (uint32_t i=2; i<size; i+=2) {
    uint16_t tile = data[i] | (data[i + 1] << 8);

    if (!writeCompressedTile(i, data, size, tile)) {
      mStream.putChar(0x00); // zero follows
      mStream.putChar(data[i]);
      mStream.putChar(data[i+1]);
    }
  }

  return true;
}

// ----------------------------------------------------------------------------
bool DialogTilesWriter::writeCompressedTile(uint32_t &i, const char *data, uint32_t size, uint16_t tile) {
  if (i + 2 >= size) {
    return false;
  }

  uint16_t nextTile = data[i + 2] | (data[i + 3] << 8);
  int diff = nextTile - tile;
  if (diff != -2 && diff != 0 && diff != 2) {
    return false;
  }

  int times = 1;
  int currentTile = nextTile;
  i += 2;

  bool toggle = false;
  if (diff == 2 && i + 2 < size) {
    uint16_t tileAfterNextTile = data[i + 2] | (data[i + 3] << 8);
    toggle = nextTile - diff == tileAfterNextTile;
  }

  while (times <= MAX_REPEAT_TIMES && (i + 2) < size) {
    if (toggle) {
      diff = -diff;
    }

    nextTile = data[i + 2] | (data[i + 3] << 8);
    if (currentTile + diff != nextTile) {
      break;
    }

    times++;
    currentTile = nextTile;
    i += 2;
  }

  uint8_t mode = 0;
  if (toggle) { mode = 0xC0; }
  else if (diff == 2) { mode = 0x40; }
  else if (diff == -2) { mode = 0x80; }

  mStream.putChar(mode | times);
  mStream.putChar(tile);
  mStream.putChar(tile >> 8);
  return true;
}

// ----------------------------------------------------------------------------
