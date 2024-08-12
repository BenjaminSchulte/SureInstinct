#pragma once

#include <QMap>
#include <fma/output/DynamicBuffer.hpp>
#include <fma/linker/LinkerBlock.hpp>

namespace Aoba {
class VariableFont;
struct VariableFontGlyph;

class VariableFontBuilder {
public:
  //! Constructor
  VariableFontBuilder(VariableFont *font) : mFont(font) {}

  //! Adds a letter
  void add(const VariableFontGlyph &letter);

  //! Builds the result into a buffer
  void build(FMA::linker::LinkerBlock *block);

protected:
  //! Writes a tile
  void writeTile(const VariableFontGlyph &letter, int x);

  //! The font
  VariableFont *mFont;

  //! List of letters and their indices
  QMap<uint32_t, uint32_t> mLetterIndex;

  //! The lowest ID
  uint32_t mLowestId = 0xFFFFFFFF;

  //! The highest ID
  uint32_t mHighestId = 0;

  //! The letter buffer
  FMA::output::DynamicBuffer mLetterBuffer;
};

}