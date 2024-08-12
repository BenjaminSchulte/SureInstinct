#include <QRect>
#include <QDebug>
#include <fma/symbol/SymbolReference.hpp>
#include <fma/symbol/CalculatedNumber.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/font/VariableFontBuilder.hpp>
#include <aoba/font/VariableFont.hpp>
#include <aoba/font/FontRegistry.hpp>
#include <aoba/yaml/YamlTools.hpp>
#include <cmath>

using namespace Aoba;

// -----------------------------------------------------------------------------
void VariableFontBuilder::add(const VariableFontGlyph &letter) {
  uint8_t numTilesX = std::ceil(letter.dataWidth / 16.0f);
  uint8_t flags = 0;
  flags |= letter.isSpace ? 0x01 : 0;

  if (flags) {
    flags |= 0x80;
  }

  uint32_t letterInIndex = mFont->assetSet()->project()->fonts()->letters().indexOf(letter.letter);
  mLowestId = qMin(letterInIndex, mLowestId);
  mHighestId = qMax(letterInIndex, mHighestId);

  mLetterIndex.insert(letterInIndex, mLetterBuffer.getSize());
  mLetterBuffer.writeNumber(letter.advance(), 1);
  mLetterBuffer.writeNumber(flags, 1);
  mLetterBuffer.writeNumber(numTilesX, 1);
  mLetterBuffer.writeNumber(letter.dataHeight, 1);
  mLetterBuffer.writeNumber(mFont->baseline() + letter.baseLine, 1);
  mLetterBuffer.writeNumber(-letter.offsetLeft, 1);
  mLetterBuffer.writeNumber(letter.advance() + letter.offsetLeft - (numTilesX - 1) * 16, 1);

  for (int x=0; x<numTilesX; x++) {
    writeTile(letter, x);
  }
}

// -----------------------------------------------------------------------------
void VariableFontBuilder::writeTile(const VariableFontGlyph &letter, int tileX) {
  int offsetX = tileX * 16;

  for (int y=0; y<letter.dataHeight; y++) {
    int pixelY = y;
    int indexY = pixelY * letter.dataWidth;
    int row = 0;
    int row2 = 0;

    for (int x=0; x<16; x++) {
      int pixelX = offsetX + x;
      int index = pixelX + indexY;
      uint16_t value = 0;
      uint16_t value2 = 0;

      if (pixelX < letter.dataWidth && pixelY < letter.dataHeight) {
        value = letter.data[index] ? 1 : 0;

        if (mFont->bitsPerPixel() > 1) {
          value2 = letter.data2[index] ? 1 : 0;
        }
      }

      row |= value << (15 - x);
      row2 |= value2 << (15 - x);
    }

    if (mFont->bitsPerPixel() > 1) {
      mLetterBuffer.writeNumber(~(row | row2), 2);
      mLetterBuffer.writeNumber(row, 2);
      mLetterBuffer.writeNumber(row2, 2);
    } else {
      mLetterBuffer.writeNumber(row, 2);
    }
  }
}

// -----------------------------------------------------------------------------
void VariableFontBuilder::build(FMA::linker::LinkerBlock *block) {
  uint32_t fallbackIndex = mFont->assetSet()->project()->fonts()->letters().indexOf(mFont->fallbackGlyph());

  uint16_t dataOffset = (mHighestId + 1) * 2;
  for (uint32_t _letter=0; _letter<=mHighestId; _letter++) {
    uint32_t letter = _letter;
    if (!mLetterIndex.contains(letter)) {
      letter = fallbackIndex;
    }

    block->write(FMA::symbol::ReferencePtr(new FMA::symbol::ConstantNumber(dataOffset + mLetterIndex[letter])), 2);
  }

  block->write(mLetterBuffer.getData(), mLetterBuffer.getSize());
}

// -----------------------------------------------------------------------------
