#include <QDebug>
#include <aoba/log/Log.hpp>
#include <QRect>
#include <aoba/font/VariableFontLoader.hpp>
#include <aoba/font/VariableFont.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool VariableFontLoader::load(const QDir &path)  {
  YAML::Node config;
  try {
    config = YAML::LoadFile(path.filePath("font.yml").toStdString().c_str());
  } catch(YAML::BadFile&) {
    return false;
  }

  mWordBreakAfter = readCharacters(config["wordBreakAfter"]);
  mSpace = readCharacters(config["space"]);

  QStringList letters(readCharacters(config["letters"]));
  mFont->setBitsPerPixel(Yaml::asInt(config["bpp"], 1));
  mFont->setBaseline(Yaml::asInt(config["baseline"], 12));
  mFont->setIsHiResFont(Yaml::asBool(config["hires"], false));
  if (letters.isEmpty()) {
    Aoba::log::error("Variable font doesn't contain any letters");
    return false;
  }

  uint32_t fallback = Yaml::asString(config["fallbackLetter"], letters[0])[0].unicode();
  mFont->setFallbackGlyph(fallback);

  if (!loadFromImage(path.filePath("default.png"), letters)) {
    return false;
  }

  if (mNumLettersLoaded != letters.size()) {
    Aoba::log::error("Different amount of letters found in image. Expected " + QString::number(letters.size()) + " according to configuration, but found " + QString::number(mNumLettersLoaded));
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
QStringList VariableFontLoader::readCharacters(const YAML::Node &config) const {
  QStringList result;
  
  if (!Yaml::isArray(config)) {
    QString row = Yaml::asString(config);
    for (int i=0; i < row.length(); i++) {
      result.push_back(row.at(i));
    }
  } else {
    for (const auto &node : config) {
      QString row = Yaml::asString(node);
      for (int i=0; i < row.length(); i++) {
        result.push_back(row.at(i));
      }
    }
  }

  return result;
}

// -----------------------------------------------------------------------------
bool VariableFontLoader::loadFromImage(const QString &fileName, const QStringList &letters) const {
  QImagePtr image(QImageAccessor::loadImage(fileName));
  if (!image) {
    Aoba::log::error("Unable to load image: " + fileName);
    return false;
  }

  int height = image->height();
  int top = -1;
  int bottom = 0;
  int index = 0;
  int step = mFont->pixelWidth();
  
  while (bottom < height) {
    bool isNewRow = image->pixelColor(0, bottom).blue() > 128 && image->pixelColor(step, bottom).blue() > 128;

    if (isNewRow) {
      if (top != -1) {
        index = parseRow(image, top, bottom - 2, index, letters);
        if (index == -1) {
          return false;
        }
      }

      top = bottom;
    }

    bottom += 2;
  }

  return true;
}

// -----------------------------------------------------------------------------
int VariableFontLoader::parseRow(const QImagePtr &image, int top, int bottom, int index, const QStringList &letters) const {
  int width = image->width();
  int left = -1;
  int right = 0;
  int step = mFont->pixelWidth();

  while (right < width) {
    bool isNewCol = image->pixelColor(right, top).blue() > 128 && image->pixelColor(right, bottom).blue() > 128;

    if (isNewCol) {
      if (left != -1 && index < letters.length()) {
        if (!parseChar(image, left, right - step, top, bottom, letters[index++])) {
          return -1;
        }
      }

      left = right;
    }

    right += step;
  }

  return index;
}

// -----------------------------------------------------------------------------
bool VariableFontLoader::parseChar(const QImagePtr &image, int left, int right, int top, int bottom, const QString &letter) const {
  int bbLeft = right;
  int bbRight = left;
  int bbTop = bottom;
  int bbBottom = top;

  int leftSep = -1;
  int rightSep = -1;
  int baseLine = -1;
  int step = mFont->pixelWidth();

  for (int tx=left; tx<=right; tx+=step) {
    bool isSepCol = image->pixelColor(tx, top).green() >= 128 && image->pixelColor(tx, top + 2).green() >= 128;
    if (!isSepCol) {
      continue;
    }

    if (leftSep == -1) {
      leftSep = tx;
    } else if (rightSep == -1) {
      rightSep = tx;
      break;
    }
  }

  for (int y=top; y<=bottom; y+=2) {
    bool isRowSep = image->pixelColor(left, y).green() >= 128 && image->pixelColor(left + step, y).green() >= 128;
    if (isRowSep) {
      baseLine = y;
    }

    for (int x=left; x<=right; x+=step) {
      bool c = image->pixelColor(x, y).red() > 0;

      if (c) {
        bbTop = qMin(bbTop, y);
        bbLeft = qMin(bbLeft, x);
        bbRight = qMax(bbRight, x);
        bbBottom = qMax(bbBottom, y);
      }
    }
  }

  if (leftSep == -1 || rightSep == -1) {
    Aoba::log::warn("Could not find left and right offset of character: " + QString(letter));
    return false;
  }
  if (baseLine == -1) {
    Aoba::log::warn("Could not find baseline of character" + QString(letter));
    return false;
  }

  if (bbLeft > bbRight || bbTop > bbBottom) {
    bbLeft = leftSep;
    bbRight = leftSep;
    bbTop = bottom;
    bbBottom = bottom;
    baseLine = bottom;
  }

  int bbHeight = bbBottom - bbTop + 2;
  int bbWidth = bbRight - bbLeft + step;

  int height = bbHeight / 2;
  int width = bbWidth / step;
  QByteArray data(width * height, 0);
  QByteArray data2(width * height, 0);

  int index = 0;
  if (mFont->bitsPerPixel() > 1) {
    for (int y=bbTop; y<=bbBottom; y+=2) {
      for (int x=bbLeft; x<=bbRight; x+=step, index++) {
        int16_t color = image->pixelColor(x, y).red() / 64;
        data[index] = color & 1;
        data2[index] = (color >> 1) & 1;
      }
    }
  } else {
    for (int y=bbTop; y<=bbBottom; y+=2) {
      for (int x=bbLeft; x<=bbRight; x+=step, index++) {
        data[index] = image->pixelColor(x, y).red();
      }
    }
  }

  VariableFontGlyph c;
  c.letter = letter[0].unicode();
  c.dataWidth = width;
  c.dataHeight = height;
  c.data = data;
  if (mFont->bitsPerPixel() > 1) {
    c.data2 = data2;
  }
  c.baseLine = (bbTop - baseLine) / 2;
  c.offsetLeft = (leftSep - bbLeft) / step;
  c.offsetRight = (rightSep - bbRight) / step;
  c.originLeft = bbLeft;
  c.originTop = bbTop;
  c.isSpace = mSpace.contains(letter);
  c.shouldWordBreakAfter = mWordBreakAfter.contains(letter);

  mFont->addGlyph(c);
  mNumLettersLoaded++;
  
  return true;
}