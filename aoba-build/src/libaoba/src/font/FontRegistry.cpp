#include <aoba/font/FontRegistry.hpp>
#include <aoba/font/Font.hpp>

using namespace Aoba; 

// -----------------------------------------------------------------------------
FontSetInterface *FontRegistry::set(const QString &id) const {
  for (FontSetInterface *set : mFontSets) {
    if (set->fontSetId() == id) {
      return set;
    }
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
FontInterface *FontRegistry::font(const QString &id) const {
  for (FontSetInterface *set : mFontSets) {
    FontInterface *font = set->font(id);
    if (font) {
      return font;
    }
  }

  return nullptr;
}