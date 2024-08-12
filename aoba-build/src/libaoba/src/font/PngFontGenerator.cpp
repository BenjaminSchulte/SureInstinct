#include <aoba/log/Log.hpp>
#include <QPainter>
#include <aoba/font/PngFontGenerator.hpp>
#include <freetype/ftbitmap.h>

using namespace Aoba;

// -----------------------------------------------------------------------------
namespace {
  const char *freeTypeErrorString(FT_Error err) {
    #undef FTERRORS_H_
    #define FT_ERRORDEF( e, v, s )  case e: return s;
    #define FT_ERROR_START_LIST     switch (err) {
    #define FT_ERROR_END_LIST       }
    #include FT_ERRORS_H
    return "(Unknown error)";
  }
}

// -----------------------------------------------------------------------------
bool PngFontGenerator::initialize() {
  FT_Error err;
  if ((err = FT_Init_FreeType(&mLibrary))) {
    Aoba::log::error("Failed to initialize freetype2 library: " + QString(freeTypeErrorString(err)));
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
QImage PngFontGenerator::build() {
  QImage result(512, 200, QImage::Format_ARGB32);
  result.fill(QColor(0,0,0,255));

  unsigned int x = 0;
  unsigned int y = 0;
  unsigned int height = mMaxPixelOffsetBottom + mMaxPixelOffsetTop + 2 ;
  unsigned int baseLine = mMaxPixelOffsetTop + 2;

  QPainter *painter = new QPainter(&result);
  painter->setCompositionMode(QPainter::CompositionMode_Plus);
  painter->setPen(QPen(QColor(0, 0, 255)));
  painter->drawLine(0, 0, 512, 0);
  painter->drawLine(0, 1, 512, 1);
  painter->drawLine(0, height, 512, height);
  painter->drawLine(0, height + 1, 512, height + 1);
  painter->setPen(QPen(QColor(0, 255, 0)));
  painter->drawLine(0, baseLine, 512, baseLine);
  painter->drawLine(0, baseLine + 1, 512, baseLine + 1);
  for (QChar letter : mLetters) {
    if (!mGlyphs.contains(letter)) {
      continue;
    }

    painter->setPen(QPen(QColor(0, 0, 255)));
    painter->drawLine(x, y, x, y + height);
    if (!mHiRes) {
      painter->drawLine(x + 1, y, x + 1, y + height);
    }

    const auto &glyph = mGlyphs[letter];
    const auto &image = glyph.image();
    unsigned int left = 2;
    unsigned int right = glyph.advanceX();
    unsigned int width = 6 + qMax<unsigned int>(glyph.width(), image.width());

    if (!mHiRes) {
      right = ((right + 1) >> 1) << 1;
      width = ((width + 1) >> 1) << 1;
    }

    if ((x + width) >= (unsigned int)result.width()) {
      x = 0;
      y += height;

      if (y + height > (unsigned int)result.height()) {
        delete painter;
        QImage old(result);
        result = QImage(512, result.height() + 200, QImage::Format_ARGB32);
        result.fill(QColor(0,0,0,255));
        painter = new QPainter(&result);
        painter->drawImage(0, 0, old);
        painter->setCompositionMode(QPainter::CompositionMode_Plus);
      }

      painter->setPen(QPen(QColor(0, 0, 255)));
      painter->drawLine(x, y, x, y + height + 1);
      painter->drawLine(0, y + height, 512, y + height);
      painter->drawLine(0, y + height + 1, 512, y + height + 1);
      if (!mHiRes) {
        painter->drawLine(x + 1, y, x + 1, y + height + 1);
      }
      painter->setPen(QPen(QColor(0, 255, 0)));
      painter->drawLine(0, y + baseLine, 512, y + baseLine);
      painter->drawLine(0, y + baseLine + 1, 512, y + baseLine + 1);
    }

    painter->setPen(QPen(QColor(0, 255, 0)));
    painter->drawLine(x + left, y, x + left, y + height - 1);
    painter->drawLine(x + right, y, x + right, y + height - 1);
    if (!mHiRes) {
      painter->drawLine(1 + x + left, y, 1 + x + left, y + height - 1);
      painter->drawLine(1 + x + right, y, 1 + x + right, y + height - 1);
    }

    int alignedY = y + baseLine - glyph.top();
    int alignedX = x + 2 + glyph.left();
    alignedY = (alignedY >> 1) << 1;
    if (!mHiRes) {
      alignedX = (alignedX >> 1) << 1;
    }
    painter->drawImage(alignedX, alignedY, glyph.image());
    
    x += width;
  }

  delete painter;
  return result;
}

// -----------------------------------------------------------------------------
bool PngFontGenerator::addFont(const QString &file, int size, bool halfSize) {
  FT_Face face;
  FT_Error err;

  if ((err = FT_New_Face(mLibrary, file.toStdString().c_str(), 0, &face))) {
    Aoba::log::error("Unable to load font " + file + ": " + QString(freeTypeErrorString(err)));
    return false;
  }

  if ((err = FT_Set_Pixel_Sizes(face, 0, size))) {
    Aoba::log::error("Unable to set pixel size:" + QString(freeTypeErrorString(err)));
    return false;
  }
  
  mFonts.push_back(PngFontGeneratorFont(face, halfSize));

  return true;
}

// -----------------------------------------------------------------------------
bool PngFontGenerator::generateMissingGlyphs() {
  for (QChar c : mLetters) {
    if (!mGlyphs.contains(c)) {
      if (!generateGlyph(c)) {
        return false;
      }
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool PngFontGenerator::generateGlyph(QChar c) {
  for (auto &font : mFonts) {
    int index = FT_Get_Char_Index(font.face(), c.unicode());
    if (index == 0) {
      continue;
    }

    return generateGlyphWithFont(c, font);
  }

  Aoba::log::warn("Could not find char " + QString(c) + " in any font");
  return false;
}

// -----------------------------------------------------------------------------
bool PngFontGenerator::generateGlyphWithFont(QChar c, PngFontGeneratorFont &font) {
  FT_Error err;
  int index = FT_Get_Char_Index(font.face(), c.unicode());

  if ((err = FT_Load_Glyph(font.face(), index, FT_LOAD_DEFAULT))) {
    Aoba::log::warn("Unable to load glyph: " + QString(freeTypeErrorString(err)));
    return false;
  }

  if ((err = FT_Render_Glyph(font.face()->glyph, FT_RENDER_MODE_MONO))) {
    Aoba::log::warn("Unable to render glyph: " + QString(freeTypeErrorString(err)));
    return false;
  }

  const auto &slot = font.face()->glyph;
  FT_Bitmap bitmap;
  FT_Bitmap_Init(&bitmap);
  if ((err = FT_Bitmap_Convert(mLibrary, &slot->bitmap, &bitmap, 1))) {
    Aoba::log::warn("Unable to convert bitmap: " + QString(freeTypeErrorString(err)));
    FT_Bitmap_Done(mLibrary, &bitmap);
    return false;
  }
  
  int targetWidth = bitmap.width;
  if (font.halfSize()) {
    targetWidth = (targetWidth + 1) >> 1;
  }
  if (!mHiRes) {
    targetWidth++;
  }
  QImage image(targetWidth, bitmap.rows + 1, QImage::Format_ARGB32);
  QColor red(255, 0, 0, 255);
  image.fill(QColor(0, 0, 0, 0));

  mMaxPixelOffsetTop = qMax(slot->bitmap_top, mMaxPixelOffsetTop);
  mMaxPixelOffsetBottom = qMax<int>(bitmap.rows - slot->bitmap_top, mMaxPixelOffsetBottom);

  for (unsigned int y=0; y<bitmap.rows; y++) {
    int rowIndex = y * bitmap.pitch;

    for (unsigned int x=0; x<bitmap.width; x++) {
      bool set = bitmap.buffer[x + rowIndex];

      if (set) {
        int toX = x;
        int toY = (y >> 1) << 1;

        if (!mHiRes) {
          toX = (x >> 1) << 1;
          if (font.halfSize()) {
            image.setPixelColor(1 + (toX >> 1), toY, red);
            image.setPixelColor(1 + (toX >> 1), toY+1, red);
          } else {
            image.setPixelColor(1 + toX, toY, red);
            image.setPixelColor(1 + toX, toY+1, red);
          }
        }

        if (font.halfSize()) {
          image.setPixelColor(toX >> 1, toY, red);
          image.setPixelColor(toX >> 1, toY+1, red);
        } else {
          image.setPixelColor(toX, toY, red);
          image.setPixelColor(toX, toY+1, red);
        }
      }
    }
  }

  mGlyphs.insert(c, PngFontGeneratorGlyph(image, slot->bitmap_left, slot->advance.x >> 6, slot->bitmap_top));

  FT_Bitmap_Done(mLibrary, &bitmap);
  return true;
}

// -----------------------------------------------------------------------------
