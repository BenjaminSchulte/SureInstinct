#include <aoba/log/Log.hpp>
#include <aoba/image/SnesPaletteImage.hpp>
#include <aoba/project/Project.hpp>

using namespace Aoba;

// ----------------------------------------------------------------------------
SnesPaletteImage::SnesPaletteImage(const PaletteImagePtr &image)
  : mImage(image)
  , mNumColors(image->numColors())
  , mAlignToColors(mNumColors)
  , mAlign(false)
{
}

// ----------------------------------------------------------------------------
uint8_t SnesPaletteImage::bitsPerPixel() const {
  switch (mNumColors) {
    case 256: return 8;
    case 16: return 4;
    case 4: return 2;

    default:
      Aoba::log::error(QString("Invalid number of colors for image: %1").arg((int)mNumColors));
      return 0;
  }
}

// ----------------------------------------------------------------------------
uint8_t SnesPaletteImage::alignedBitsPerPixel() const {
  switch (mAlignToColors) {
    case 256: return 8;
    case 16: return 4;
    case 4: return 2;

    default:
      Aoba::log::error(QString("Invalid aligned colors for image: %1").arg((int)mNumColors));
      return 0;
  }
}

// ----------------------------------------------------------------------------
bool SnesPaletteImage::validateImage() const {
  if (mImage->width() % 8 != 0) {
    Aoba::log::error(QString("Image width must be dividable by 8"));
    return false;
  }
  
  if (mImage->height() % 8 != 0) {
    Aoba::log::error(QString("Image height must be dividable by 8"));
    return false;
  }

  return true;
}

// ----------------------------------------------------------------------------
unsigned int SnesPaletteImage::requiredMemory() const {
  uint8_t bpp = bitsPerPixel();
  uint8_t alignedBpp = alignedBitsPerPixel();

  unsigned int bytesPerAlignedColorRow = alignedBpp * 16 * 8;
  unsigned int bufferSize = (mImage->width() * mImage->height() / 8) * bpp;

  if (mAlign) {
    bufferSize = ((bufferSize + bytesPerAlignedColorRow - 1) / bytesPerAlignedColorRow) * bytesPerAlignedColorRow;
  }

  return bufferSize;
}

// ----------------------------------------------------------------------------
QByteArray SnesPaletteImage::compileColorImage256(uint32_t, uint32_t, uint8_t colorOffset) const {
  // Prepare data
  unsigned int width = mImage->width();
  unsigned int height = mImage->height();

  QByteArray buffer;
  buffer.resize(width * height);
  uint32_t index=0;
  for (unsigned int y=0; y<height; y+=8) {
    for (unsigned int x=0; x<width; x+=8) {
      for (unsigned int by=0; by<8; by++) {
        for (unsigned int bx=0; bx<8; bx++) {
          buffer[index++] = mImage->colorAt(x + bx, y + by) + colorOffset;
        }
      }
    }
  }
  
  return buffer;
}

// ----------------------------------------------------------------------------
QByteArray SnesPaletteImage::compile(uint32_t tileWidth, uint32_t tileHeight, uint8_t colorOffset) const {
  if (!validateImage()) {
    return QByteArray();
  }

  /*if (mImage->numColors() == 256) {
    return compileColorImage256(tileWidth, tileHeight, colorOffset);
  }*/

  // Precalculate values
  uint8_t bpp = bitsPerPixel();
  uint8_t alignedBpp = alignedBitsPerPixel();
  if (bpp == 0 || alignedBpp == 0) {
    return QByteArray();
  }

  // Prepare data
  QByteArray buffer;
  buffer.resize(requiredMemory());

  char *data = buffer.data();

  unsigned int width = mImage->width();
  unsigned int height = mImage->height();
  unsigned int numTilesX = width / tileWidth;
  unsigned int bgTilesPerTileX = tileWidth / 8;
  unsigned int bgTilesPerTileY = tileHeight / 8;

  unsigned int bytesPerTileX = 8 * bpp;
  unsigned int bytesPerTileY = bytesPerTileX * 16;
  uint8_t bytes[8];

  for (unsigned int y=0; y<height; y++) {
    unsigned int by = y / 8;
    unsigned int oy = y % 8;

    for (unsigned int x=0; x<width; x+=8) {
      unsigned int bx = x / 8;

      *(reinterpret_cast<uint64_t*>(&bytes)) = 0;

      for (unsigned int b=0; b<8; b++) {
        uint8_t color = mImage->colorAt(x + b, y) + colorOffset;

        bytes[0] |= ((color >> 0) & 1) << (7 - b);
        bytes[1] |= ((color >> 1) & 1) << (7 - b);
        bytes[2] |= ((color >> 2) & 1) << (7 - b);
        bytes[3] |= ((color >> 3) & 1) << (7 - b);
        bytes[4] |= ((color >> 4) & 1) << (7 - b);
        bytes[5] |= ((color >> 5) & 1) << (7 - b);
        bytes[6] |= ((color >> 6) & 1) << (7 - b);
        bytes[7] |= ((color >> 7) & 1) << (7 - b);
      }

      unsigned int currentTile = (bx / bgTilesPerTileX) + (by / bgTilesPerTileY) * numTilesX;
      unsigned int currentTileSubX = bx % bgTilesPerTileX;
      unsigned int currentTileSubY = by % bgTilesPerTileY;
      unsigned int currentTileMainX = currentTile % (16 / bgTilesPerTileX);
      unsigned int currentTileMainY = currentTile / (16 / bgTilesPerTileX);

      unsigned int firstByteX = currentTileMainX * bytesPerTileX * bgTilesPerTileX + currentTileSubX * bytesPerTileX;
      unsigned int firstByteY = currentTileMainY * bytesPerTileY * bgTilesPerTileY + currentTileSubY * bytesPerTileY;
      unsigned int firstByte = firstByteX + firstByteY + oy * 2;

      data[firstByte + 0] = bytes[0];
      data[firstByte + 1] = bytes[1];

      if (bpp >= 4) {
        data[firstByte + 16] = bytes[2];
        data[firstByte + 17] = bytes[3];
      }
      if (bpp >= 8) {
        data[firstByte + 32] = bytes[4];
        data[firstByte + 33] = bytes[5];
        data[firstByte + 48] = bytes[6];
        data[firstByte + 49] = bytes[7];
      }

    }
  }

  return buffer;
}