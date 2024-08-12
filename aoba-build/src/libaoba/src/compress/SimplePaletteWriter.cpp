#include <aoba/log/Log.hpp>
#include <aoba/compress/SimplePaletteWriter.hpp>
#include <aoba/project/Project.hpp>

using namespace Aoba;

// ----------------------------------------------------------------------------
bool SimplePaletteWriter::write(const char *data, uint32_t size) {
  for (uint32_t i=0; i<size; i++) {
    writeByte(data[i]);
  }

  return true;
}

// ----------------------------------------------------------------------------
void SimplePaletteWriter::writeByte(uint8_t byte) {
  if (mLastColorByte == -1) {
    mLastColorByte = byte;
  } else {
    writeColor(mLastColorByte | ((int)byte << 8));
    mLastColorByte = -1;
  }
}

// ----------------------------------------------------------------------------
void SimplePaletteWriter::writeColor(uint16_t color) {
  int8_t r = color & 0x1F;
  int8_t g = (color >> 5) & 0x1F;
  int8_t b = (color >> 10) & 0x1F;

  int8_t diffR = r - mLastR;
  int8_t diffG = g - mLastG;
  int8_t diffB = b - mLastB;

  if (diffR == 0 && diffG == 0 && diffB == 0) {
    if (++mRepeatCounter == 255) {
      flushRepeat();
    }
  } else if (diffR >= 0 && diffR < 4 && diffG >= 0 && diffG < 4 && diffB >= 0 && diffB < 4) {
    flushRepeat();
    mBuffer.append(0x80 | diffR | (diffG << 2) | (diffB << 4));
  } else if (diffR <= 0 && diffR > -4 && diffG <= 0 && diffG > -4 && diffB <= 0 && diffB > -4) {
    mBuffer.append(0xC0 | -diffR | (-diffG << 2) | (-diffB << 4));
  } else {
    flushRepeat();
    mBuffer.append(color >> 8);
    mBuffer.append(color);
  }


  mLastR = r;
  mLastG = g;
  mLastB = b;
}

// ----------------------------------------------------------------------------
void SimplePaletteWriter::flushRepeat() {
  if (mRepeatCounter == 0) {
    return;
  }

  mBuffer.append((char)0x80);
  mBuffer.append(mRepeatCounter);

  mRepeatCounter = 0;
}

// ----------------------------------------------------------------------------
bool SimplePaletteWriter::finalize() {
  flushRepeat();
  return true;
}