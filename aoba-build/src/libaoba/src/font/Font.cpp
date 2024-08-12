#include <aoba/font/Font.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
FontInterface *FontSetInterface::font(const QString &id) const {
  for (FontInterface *font : allFonts()) {
    if (font->fontId() == id) {
      return font;
    }
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
void FontSetInterface::addUsedLetter(uint32_t letter, uint32_t usage) {
  mLetters[letter] += usage;
}

// -----------------------------------------------------------------------------
void FontInterface::addUsedLetter(uint32_t letter, uint32_t usage) {
  mLetters[letter] += usage;
  fontSet()->addUsedLetter(letter, usage);
}

// -----------------------------------------------------------------------------
