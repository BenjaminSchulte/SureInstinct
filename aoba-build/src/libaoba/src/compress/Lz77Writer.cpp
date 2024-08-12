#include <aoba/log/Log.hpp>
#include <aoba/compress/Lz77Writer.hpp>
#include <aoba/compress/Lz77Compressor.hpp>

using namespace Aoba;

// ----------------------------------------------------------------------------
bool Lz77Writer::write(const char *data, uint32_t size) {
  for (uint32_t i=0; i<size; i+=mMaxBytesPerSection) {
    uint32_t bytesToWrite = std::min(size - i, mMaxBytesPerSection);

    writePart(data + i, bytesToWrite, bytesToWrite < (size - i));
  }

  return true;
}

// ----------------------------------------------------------------------------
void Lz77Writer::writePart(const char *data, uint32_t size, bool hasMore) {
  Lz77Compressor compressor(4, 4);
  compressor.write(data, size);
  compressor.close();

  uint32_t totalCommands = compressor.numSingleBytes() + compressor.numLookUps();
  uint32_t totalSize = ((totalCommands + 7) / 8) * 9;
  uint32_t offset = mCurrentBufferOffset;

  mBuffer.resize(offset + 2 + totalSize);
  char *buffer = mBuffer.data() + offset;
  
  *(reinterpret_cast<uint16_t*>(&buffer[0])) = size | (hasMore ? 0x8000 : 0);
  buffer += 2;

  char *headerByte;
  uint8_t currentByte = 8;

  for (const auto &part : compressor.stream()) {
    if (currentByte == 8) {
      headerByte = buffer++;
      *headerByte = 0;
      currentByte = 0;
    }

    switch (part.type) {
      case Lz77CompressorPart::BYTE:
        *(buffer++) = part.byte;
        break;

      case Lz77CompressorPart::LOOKUP:
        *(buffer++) = part.offset | (part.length << 4);
        *headerByte |= 1 << currentByte;
        break;
      
      default:
        Aoba::log::fatal("Fatal error: invalid lz77 compression part type");
        return;
    }

    currentByte++;
  }

  mCurrentBufferOffset = (buffer - mBuffer.data());
}