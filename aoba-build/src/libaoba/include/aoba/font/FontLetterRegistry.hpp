#pragma once

#include <QString>
#include <QMap>
#include "../text/TextProperties.hpp"

namespace Aoba {
class FontRegistry;

typedef QMap<uint32_t, int> FontLetterIndex;

class FontLetterRegistry {
public:
  //! Builds the index
  void buildIndex(FontRegistry *reg);
  
  //! Returns the letter in the index
  inline int indexOf(QChar letter) const {
    uint32_t i = letter.unicode();
    return mLetterIndex.contains(i) ? mLetterIndex[i] : -1;
  }

  //! Returns the number of bytes for a char
  inline int numBytes(QChar letter) const {
    return indexOf(letter) < TEXT_NUM_ONE_BYTE_LETTERS;
  }

private:
  //! Whether the index has been built
  bool mIndexIsBuilt = false;

  //! Map letter -> letter index
  QMap<uint32_t, int> mLetterIndex;
};

}