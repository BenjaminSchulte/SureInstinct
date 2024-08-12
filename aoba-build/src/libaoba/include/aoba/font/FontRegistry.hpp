#pragma once

#include <QVector>
#include "FontLetterRegistry.hpp"

namespace Aoba {
class FontInterface;
class FontSetInterface;

class FontRegistry {
public:
  //! Adds a font set
  inline void registerFontSet(FontSetInterface *set) {
    mFontSets.push_back(set);
  }

  //! Returns all sets
  inline const QVector<FontSetInterface*> &allSets() const { return mFontSets; }

  //! Returns the letters
  inline FontLetterRegistry &letters() { return mLetters; }

  //! Returns a specific set
  FontSetInterface *set(const QString &id) const;

  //! Returns a font
  FontInterface *font(const QString &id) const;

protected:
  //! Font letter registry
  FontLetterRegistry mLetters;

  //! List of all font sets
  QVector<FontSetInterface*> mFontSets;
};

}