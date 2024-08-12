#pragma once

#include <QString>
#include <QVector>
#include <QMap>
#include <QImage>
#include <ft2build.h>
#include FT_FREETYPE_H

namespace Aoba {

struct PngFontGeneratorFont {
  //! Constructor
  PngFontGeneratorFont() {}

  //! Constructor
  PngFontGeneratorFont(const FT_Face &face, bool halfSize) : mFace(face), mHalfSize(halfSize) {}

  //! Returns the face
  inline FT_Face &face() { return mFace; }

  //! Returns whether half size is used
  inline bool halfSize() const { return mHalfSize; }

private:
  //! The font
  FT_Face mFace;

  //! Whether to use half size fonts
  bool mHalfSize;
};

struct PngFontGeneratorGlyph {
  //! Constructor
  PngFontGeneratorGlyph() {}

  //! Constructor
  PngFontGeneratorGlyph(const QImage &image, int left, int advanceX, int top)
    : mImage(image), mLeft(left), mAdvanceX(advanceX), mTop(top) {}

  //! The image
  inline const QImage &image() const { return mImage; }

  //! Negative only left
  inline int negativeOnlyLeft() const { return qMax(mLeft, 0); }

  //! Left start
  inline int left() const { return mLeft; }

  //! Advance X
  inline int advanceX() const { return mAdvanceX; }

  //! The top position
  inline int top() const { return mTop; }

  //! Total widht
  inline int width() const { return mAdvanceX - negativeOnlyLeft(); }

private:
  //! The image
  QImage mImage;

  //! Left start
  int mLeft;

  //! Advance X
  int mAdvanceX;

  //! The top position
  int mTop;
};

class PngFontGenerator {
public:

  //! Initializes the generator
  bool initialize();

  //! Adds a font
  bool addFont(const QString &file, int size, bool halfSize=false);

  //! Generates the output image
  QImage build();

  //! Generates all missing glyps
  bool generateMissingGlyphs();

  //! Generates all missing glyps
  bool generateGlyph(QChar c);

  //! Generates all missing glyps
  bool generateGlyphWithFont(QChar c, PngFontGeneratorFont &);

  //! Sets the letters to generate
  inline void setLetters(const QString &letters) { mLetters = letters; }

  //! Sets if a hires font should be generated
  inline void setHiRes(bool hires) { mHiRes = hires; }

private:
  //! List of all letters to generate
  QString mLetters;

  //! Whether the generator should generate hires font
  bool mHiRes = false;

  //! List of all fonts
  QVector<PngFontGeneratorFont> mFonts;

  //! List of all glyphs
  QMap<QChar, PngFontGeneratorGlyph> mGlyphs;

  //! The maximum pixel offset
  int mMaxPixelOffsetTop = 0;

  //! The maximum height
  int mMaxPixelOffsetBottom = 0;

  //! The library
  FT_Library mLibrary;
};

}