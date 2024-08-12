#pragma once

#include <QMap>
#include <QVector>

#include "FontLetterRegistry.hpp"

namespace Aoba {

class FontInterface;

class FontSetInterface {
public:
  //! Constructor
  FontSetInterface() = default;
  
  //! Deconstructor
  virtual ~FontSetInterface() = default;

  //! Returns the ID of this font
  virtual QString fontSetId() const = 0;

  //! Returns all fonts
  virtual QVector<FontInterface*> allFonts() const = 0;

  //! Adds supported letters
  void addUsedLetter(uint32_t letter, uint32_t usage=1);

  //! Returns the letter usage
  inline const FontLetterIndex &letterUsage() const { return mLetters; }

  //! Returns a specific set
  FontInterface *font(const QString &id) const;

protected:
  //! List of all used letters
  FontLetterIndex mLetters;
};

class FontInterface {
public:
  //! Constructor
  FontInterface() = default;
  
  //! Deconstructor
  virtual ~FontInterface() = default;

  //! Returns the ID of this font
  virtual QString fontId() const = 0;

  //! Returns the font set
  virtual FontSetInterface *fontSet() const = 0;

  //! Adds supported letters
  void addUsedLetter(uint32_t letter, uint32_t usage=1);

protected:
  //! List of all used letters
  FontLetterIndex mLetters;
};

}