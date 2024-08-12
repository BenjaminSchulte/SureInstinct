#include <aoba/log/Log.hpp>
#include <aoba/compress/MapTilesWriter.hpp>
#include <aoba/project/Project.hpp>

using namespace Aoba;

// ----------------------------------------------------------------------------
MapTilesWriter::MapTilesWriter(Project *project)
  : AbstractWriter(project)
  , mStream(&mBuffer)
{
}

// ----------------------------------------------------------------------------
bool MapTilesWriter::write(const char *data, uint32_t size) {
  if (!mStream.open(QIODevice::WriteOnly)) {
    Aoba::log::error("Unable to open output stream for writing.");
    return false;
  }

  if (size < 2) {
    Aoba::log::error("Missing layer header required by tilemap_tiles compressor.");
    return false;
  }

  uint16_t width = *reinterpret_cast<const uint8_t*>(&data[0]) + 1;
  uint16_t height = *reinterpret_cast<const uint8_t*>(&data[1]) + 1;
  if (size != (uint32_t)(width * height * 2 + 2)) {
    Aoba::log::error("Data size for tilemap_tiles compressor is invalid.");
    return false;
  }

  // mStream.putChar(width);
  // mStream.putChar(height);

  const char *input = data + 2;
  uint32_t x = 0;
  uint32_t y = 0;
  do {
    uint32_t index = (x + y * width) * 2;
    uint32_t result = writeTile(input, index, x, y, width, height);
    if (result == 0) {
      Aoba::log::error("Unable to compress tilemap_tiles. Please choose a different compressor type.");
      return false;
    }
    x += result;

    if (x >= width) {
      x -= width;
      y++;
    }
  } while (y < height);

  return true;
}

// ----------------------------------------------------------------------------
uint32_t MapTilesWriter::countReferenceSize(const char *input, uint32_t i, uint32_t j, uint32_t x, uint16_t w, uint16_t h) const {
  uint32_t size = 1;
  uint32_t bufferLength = w * h * 2;

  while (size < 17 && ++x < w) {
    i += 2;
    j += 2;

    if (i >= bufferLength || j >= bufferLength) {
      break;
    }

    uint16_t left = input[i] | (input[i + 1] << 8);
    uint16_t right = input[j] | (input[j + 1] << 8);
    if (left != right) {
      break;
    }

    size++;
  }

  return size;
}

// ----------------------------------------------------------------------------
MapTilesWriter::Reference MapTilesWriter::findReference(const char *input, int32_t x, int32_t y, uint16_t w, uint16_t h, int ox, int oy) const {
  Reference ref;
  int offset = -1;
  
  int currentIndex = (x + y * w) * 2;
  uint16_t current = input[currentIndex] | (input[currentIndex + 1] << 8);

  do {
    offset++;
    x += ox;
    y += oy;
    if (x < 0 || y < 0) { return ref; }

    uint32_t i = (x + y * w) * 2;
    if (i >= (uint32_t)(w * h * 2)) {
      return ref;
    }

    uint16_t data = input[i] | (input[i + 1] << 8);
    if (data != current) {
      continue;
    }

    uint32_t size = countReferenceSize(input, i, currentIndex, x, w, h);
    if (size > ref.size) {
      ref.offset = offset;
      ref.size = size;
    }
  } while (offset < 15);

  return ref;
}

// ----------------------------------------------------------------------------
uint32_t MapTilesWriter::writeTile(const char *input, uint32_t index, uint32_t x, uint32_t y, uint16_t w, uint32_t h) {
  Reference refX = findReference(input, x, y, w, h, -1, 0);
  Reference refY = findReference(input, x, y, w, h, 0, -1);

  if (refX.size > 1 && refX.size >= refY.size) {
    mStream.putChar((char)0xFB);
    mStream.putChar((refX.size - 2) | ((refX.offset) << 4));
    return refX.size;
  } else if (refY.size > 1 && refY.size > refX.size) {
    mStream.putChar((char)0xFC);
    mStream.putChar((refY.size - 2) | ((refY.offset) << 4));
    return refY.size;
  } else {
    if (writeTileDirect(input, index)) {
      return 1;
    } else {
      return 0;
    }
  }
}

// ----------------------------------------------------------------------------
bool MapTilesWriter::writeTileDirect(const char *_input, uint32_t index) {
  const unsigned char *input = reinterpret_cast<const unsigned char *>(_input);
  uint8_t flags = input[index + 1];
  uint16_t tile = input[index] | (flags & 0x01);

  flags &= 0xC0;

  if (flags == 0) {
    if (tile >= 0xF8) {
      mStream.putChar(0xFD | (tile >> 8));
      mStream.putChar(tile);
    } else {
      mStream.putChar(tile);
    }
  } else if (tile >= 0x100) {
    return false;
  } else if (flags == 0x40) {
    mStream.putChar((char)0xF8);
    mStream.putChar(tile);
  } else if (flags == 0x80) {
    mStream.putChar((char)0xF9);
    mStream.putChar(tile);
  } else if (flags == 0xC0) {
    mStream.putChar((char)0xFA);
    mStream.putChar(tile);
  }

  return true;
}