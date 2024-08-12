#pragma once

#include <QVector>

namespace FMA {
namespace linker {
class LinkerBlock;
}
}

namespace Aoba {
class FontLetterRegistry;
class TextRegistry;

struct TextDictionaryWord {
  QVector<QChar> word;
  int count = 0;
  int score = 0;
  QVector<uint32_t> indices;
  QMap<QChar, TextDictionaryWord> children;

  QString asString() const;
};

class TextDictionary {
public:

  //! Adds the content from the dictionary 
  void setTextDictionary(const QVector<TextDictionaryWord> &dict);

  //! Adds a word
  void addWord(const QVector<QChar> &word);

  //! Writes the end of a text char
  void writeEndOfText(FMA::linker::LinkerBlock*) const;

  //! Writes content to an output stream
  void write(FMA::linker::LinkerBlock*, const QString &text, FontLetterRegistry *letters, bool compressed) const;

  //! Writes content to an output stream
  void writeChar(FMA::linker::LinkerBlock*, QChar c, FontLetterRegistry *letters) const;

  //! Writes a reference
  void writeReference(FMA::linker::LinkerBlock*, int ref) const;

  //! Looks up an item in the dict
  bool searchDictionary(const QString &text, int *result, int &startAt) const;

  //! Builds the dictionary
  bool build(TextRegistry *reg, FontLetterRegistry *letters, uint8_t bank) const;

private:
  //! Index
  struct TextDictionaryIndex {
    ~TextDictionaryIndex() {
      for (TextDictionaryIndex *child : children) {
        delete child;
      }
    }

    int index = -1;
    QMap<QChar, TextDictionaryIndex*> children;
  };

  //! List of all words
  QVector<QVector<QChar>> mWords;

  //! The word index
  TextDictionaryIndex mIndex;
};

}