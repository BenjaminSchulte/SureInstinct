#pragma once

#include <aoba/image/QImageAccessor.hpp>
#include <QString>
#include <QDir>
#include <yaml-cpp/yaml.h>

namespace Aoba {
class VariableFont;

class VariableFontLoader {
public:
  //! Constructor
  VariableFontLoader(VariableFont *font) : mFont(font) {}

  //! Loads from a specific image
  bool load(const QDir &path);
  
protected:
  //! Loads data from a image
  bool loadFromImage(const QString &image, const QStringList &letters) const;

  //! Parses a row
  int parseRow(const QImagePtr &image, int top, int bottom, int index, const QStringList &letters) const;

  //! Parses a row
  bool parseChar(const QImagePtr &image, int left, int right, int top, int bottom, const QString &letter) const;

  //! Returns a string list
  QStringList readCharacters(const YAML::Node &) const;

  //! The font
  VariableFont *mFont;
  
  //! List of all word break letters
  QStringList mWordBreakAfter;

  //! List of all space characters
  QStringList mSpace;

  //! Number of letters loaded
  mutable int mNumLettersLoaded = 0;
};

}