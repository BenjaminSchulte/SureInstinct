#pragma once

#include <QMap>
#include <QVector>
#include "TextDictionary.hpp"

namespace Aoba {
class TextRegistry;
class FontLetterRegistry;

class TextDictionaryBuilder {
public:
  //! Constructor
  TextDictionaryBuilder(TextDictionary *dict, FontLetterRegistry *letters);

  //! Builds the dictionary
  bool build(TextRegistry *reg);

private:

  //! Builds the final dictionary
  QVector<TextDictionaryWord> buildFinalDictionary(const QVector<TextDictionaryWord> &queue);

  //! Builds the dictionary of all possible words with its score
  QVector<TextDictionaryWord> buildFullDictionary(const QVector<TextDictionaryWord> &previewDict);

  //! Builds a single item
  void buildFullDictionaryItem(const TextDictionaryWord &item, QVector<TextDictionaryWord> &fullDict, const QVector<uint32_t> &indices);

  //! Builds the preview dictionary
  QVector<TextDictionaryWord> buildPreviewDictionary();

  //! Flattens the preview dictionary
  QVector<TextDictionaryWord> flattenDictionary(const TextDictionaryWord &root);

  //! Flattens the preview dictionary
  void recursiveFlattenDictionary(const QString &context, QVector<TextDictionaryWord> &result, const TextDictionaryWord &root);

  //! Adds all texts from the registry
  void addAllTextsFromRegistry(TextRegistry *reg);

  //! Scores an item
  int scoreItem(const TextDictionaryWord &item) const;

  //! Counts occurrences
  QVector<uint32_t> countOccurrences(const QVector<uint32_t> &indices, uint32_t length);

  //! Marks the buffer as being used
  void markBufferProcessed(const QVector<uint32_t> &indices, uint32_t length);

  //! Inserts an item back to the queue
  void insertToQueue(QVector<TextDictionaryWord> &queue, const TextDictionaryWord &word);

  //! The full buffer containing all texts concatinated
  QString mFullTextBuffer;

  //! The dictionary
  TextDictionary *mDictionary;

  //! The letter dictionary
  FontLetterRegistry *mLetters;
};

}