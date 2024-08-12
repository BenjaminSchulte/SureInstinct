#pragma once

#include <QDir>
#include "../image/ImageAccessor.hpp"
#include "../asset/Asset.hpp"
#include "../asset/LinkedAssetSet.hpp"
#include "./Font.hpp"

namespace Aoba {
class VariableFont;

struct VariableFontGlyph {
  uint32_t letter;
  int dataWidth;
  int dataHeight;
  QByteArray data;
  QByteArray data2; // for 2bpp
  int baseLine;
  int offsetLeft;
  int offsetRight;
  int originLeft;
  int originTop;
  bool isSpace = false;
  bool shouldWordBreakAfter = false;
  
  inline int advance() const {
    return offsetRight + dataWidth - offsetLeft;
  }
  
};

class VariableFontAssetSet : public LinkedAssetSet<VariableFont>, public FontSetInterface {
public:
  //! Constructor
  VariableFontAssetSet(Project *project, const QString &id)
    : LinkedAssetSet<VariableFont>("variable_font", project)
    , mId(id)
  {}

  //! Returns the set ID
  QString fontSetId() const override { return mId; }

  //! Returns all fonts
  QVector<FontInterface*> allFonts() const override;

protected:
  //! The ID
  QString mId;
};

class VariableFont : public Asset<VariableFontAssetSet>, public FontInterface {
public:
  //! Constructor
  VariableFont(VariableFontAssetSet *set, const QString &id, const QDir &dir);

  //! Returns the path
  inline const QDir &path() const { return mPath; }

  //! Returns the set ID
  QString fontId() const override { return mId; }

  //! Loads the asset
  bool reload() override;

  //! Builds the asset
  bool build() override;

  //! Returns the necessary FMA code
  QString getFmaCode() const override;

  //! Returns additional object files to include
  QStringList getFmaObjectFiles() const override;

  //! Returns the font index symbol name
  QString fontIndexSymbolName() const;

  //! Returns the font set
  FontSetInterface *fontSet() const override {
    return mAssetSet;
  }

  //! Returns whether this is a hires font
  inline bool isHiResFont() const { return mHiResFont; }

  //! Sets the hires font flag
  void setIsHiResFont(bool f) { mHiResFont = f; }

  //! Returns the pixel width
  inline int pixelWidth() const { return mHiResFont ? 1 : 2; }

  //! Adds a glyph
  inline void addGlyph(const VariableFontGlyph &glyph) {
    mGlyphs.insert(glyph.letter, glyph);
  }

  //! Sets the fallback glyph
  inline void setFallbackGlyph(uint32_t glyph) {
    mFallbackGlyph = glyph;
  }

  //! Sets the fallback glyph
  inline uint32_t fallbackGlyph() const { return mFallbackGlyph; }

  //! Sets the bits per pixel
  inline void setBitsPerPixel(uint8_t bpp) { mBitsPerPixel = bpp; }

  //! Sets the bits per pixel
  uint8_t bitsPerPixel() const { return mBitsPerPixel; }

  //! Sets the bits per pixel
  inline void setBaseline(uint8_t baseline) { mBaseline = baseline; }

  //! Sets the bits per pixel
  uint8_t baseline() const { return mBaseline; }

protected:
  //! The containing file
  QDir mPath;

  //! Whether the font is a hires font
  bool mHiResFont;

  //! Map with all characters
  QMap<uint32_t, VariableFontGlyph> mGlyphs;

  //! The fallback character
  uint32_t mFallbackGlyph = 0;

  //! Number of bits per pixel
  uint8_t mBitsPerPixel = 1;

  //! Baseline position
  uint8_t mBaseline = 12;
};

}