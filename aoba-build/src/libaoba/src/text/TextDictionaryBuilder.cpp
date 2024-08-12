#include <aoba/log/Log.hpp>
#include <aoba/text/TextProperties.hpp>
#include <aoba/text/TextMessage.hpp>
#include <aoba/text/TextRegistry.hpp>
#include <aoba/text/TextDictionary.hpp>
#include <aoba/text/TextDictionaryBuilder.hpp>
#include <aoba/asset/AssetLinkerObject.hpp>

using namespace Aoba;


// -----------------------------------------------------------------------------
TextDictionaryBuilder::TextDictionaryBuilder(TextDictionary *dict, FontLetterRegistry *letters)
  : mDictionary(dict)
  , mLetters(letters)
{
}

// -----------------------------------------------------------------------------
bool TextDictionaryBuilder::build(TextRegistry *reg) {
  addAllTextsFromRegistry(reg);

  mDictionary->setTextDictionary(buildFinalDictionary(buildFullDictionary(buildPreviewDictionary())));

  return true;
}
// -----------------------------------------------------------------------------
QVector<uint32_t> TextDictionaryBuilder::countOccurrences(const QVector<uint32_t> &indices, uint32_t length) {
  uint32_t minIndex = 0;
  QVector<uint32_t> result;

  for (uint32_t index : indices) {
    if (index < minIndex) {
      continue;
    }

    bool valid = true;
    for (uint32_t i=0; i<length; i++) {
      if (mFullTextBuffer[index + i] == '\0') {
        valid = false;
        break;
      }
    }

    if (valid) {
      result.push_back(index);
      minIndex = index + length;
    }
  }

  return result;
}

// -----------------------------------------------------------------------------
void TextDictionaryBuilder::insertToQueue(QVector<TextDictionaryWord> &queue, const TextDictionaryWord &word) {
  int queueSize = queue.length();
  int newScore = word.score;

  int left = 0;
  int right = queueSize - 1;

  while (right >= left) {
    int index = ((right - left) >> 1) + left;
    int thisScore = queue[index].score;

    if (thisScore < newScore) {
      right = index - 1;
    } else if (thisScore > newScore) {
      left = index + 1;
    } else {
      queue.insert(index, word);
      return;
    }
  }

  queue.insert(left, word);
}

// -----------------------------------------------------------------------------
void TextDictionaryBuilder::markBufferProcessed(const QVector<uint32_t> &indices, uint32_t length) {
  for (uint32_t index : indices) {
    for (uint32_t i=0; i<length; i++) {
      mFullTextBuffer[index + i] = '\0';
    }
  }
}

// -----------------------------------------------------------------------------
QVector<TextDictionaryWord> TextDictionaryBuilder::buildFinalDictionary(const QVector<TextDictionaryWord> &_queue) {
  QVector<TextDictionaryWord> queue(_queue);
  QVector<TextDictionaryWord> result;
  std::sort(queue.begin(), queue.end(), [](const TextDictionaryWord &a, const TextDictionaryWord &b){ return b.score < a.score;});

  while (queue.length() && result.length() < TEXT_DICT_SIZE) {
    TextDictionaryWord item(queue.front());
    queue.pop_front();

    QVector<uint32_t> newIndices(countOccurrences(item.indices, item.word.length()));
    if (newIndices.length() < TEXT_MIN_OCCURRENCE) {
      continue;
    }

    // Did the amount of found items change?
    if (newIndices.length() != item.count) {
      item.count = newIndices.length();
      item.indices = newIndices;
      item.score = scoreItem(item);
      insertToQueue(queue, item);
      continue;
    }

    //! Use this item
    result.push_back(item);
    markBufferProcessed(newIndices, item.word.length());
  }

  return result;
}

// -----------------------------------------------------------------------------
void TextDictionaryBuilder::buildFullDictionaryItem(const TextDictionaryWord &item, QVector<TextDictionaryWord> &result, const QVector<uint32_t> &_lookupIndices) {
  if (item.word.length() >= TEXT_MAX_DICT_LENGTH) {
    return;
  }

  QVector<uint32_t> lookupIndices(_lookupIndices);
  std::sort(lookupIndices.begin(), lookupIndices.end());
  while (lookupIndices.length()) {
    uint32_t firstIndex = lookupIndices.front();
    lookupIndices.pop_front();
    uint32_t wordLength = item.word.length();
    if (firstIndex + wordLength >= (uint32_t)mFullTextBuffer.length()) {
      continue;
    }

    QChar letter = mFullTextBuffer[firstIndex + wordLength];
    if (letter == '\0') {
      continue;
    }

    TextDictionaryWord newItem;
    newItem.word = item.word;
    newItem.word.push_back(letter);
    newItem.count = 1;
    newItem.indices.push_back(firstIndex);

    uint32_t minIndex = firstIndex + wordLength + 1;
    QVector<uint32_t> missingIndices;
    for (uint32_t index : lookupIndices) {
      if (index < minIndex) {
        continue;
      }

      if (index + wordLength >= (uint32_t)mFullTextBuffer.length()) {
        continue;
      }

      if (mFullTextBuffer[index + wordLength] != letter) {
        missingIndices.push_back(index);
        continue;
      }

      minIndex = index + wordLength + 1;
      newItem.count++;
      newItem.indices.push_back(index);
    }

    if (newItem.count >= TEXT_MIN_OCCURRENCE) {
      result.push_back(newItem);

      buildFullDictionaryItem(newItem, result, newItem.indices);
    }

    lookupIndices = missingIndices;
  }
}

// -----------------------------------------------------------------------------
QVector<TextDictionaryWord> TextDictionaryBuilder::buildFullDictionary(const QVector<TextDictionaryWord> &previewDict) {
  QVector<TextDictionaryWord> fullDict;

  for (const auto &item : previewDict) {
    fullDict.push_back(item);
    buildFullDictionaryItem(item, fullDict, item.indices);
  }

  for (auto &item : fullDict) {
    item.score = scoreItem(item);
  }

  return fullDict;
}

// -----------------------------------------------------------------------------
int TextDictionaryBuilder::scoreItem(const TextDictionaryWord &item) const {
  int numBytes = 0;
  for (const QChar &letter : item.word) {
    numBytes += mLetters->numBytes(letter);
  }

  return item.count * (numBytes - 2) - numBytes - 2;
}

// -----------------------------------------------------------------------------
QVector<TextDictionaryWord> TextDictionaryBuilder::buildPreviewDictionary() {
  // Builds a dictionary containing all words which have the minimum length for a dictionary item
  TextDictionaryWord dict;

  uint32_t bufferLength = mFullTextBuffer.length();
  for (uint32_t start=0; start<bufferLength; start++) {
    TextDictionaryWord *scope = &dict;
    scope->count++;

    for (uint32_t offset=0; offset<TEXT_MIN_DICT_LENGTH; offset++) {
      if (start + offset >= bufferLength) {
        break;
      }

      QChar letter = mFullTextBuffer[start + offset];
      if (letter == '\0') {
        break;
      }

      if (!scope->children.contains(letter)) {
        scope->children.insert(letter, TextDictionaryWord());
      }

      if (offset == TEXT_MIN_DICT_LENGTH - 1) {
        scope->children[letter].indices.push_back(start);
      }

      scope = &scope->children[letter];
      scope->count++;
    }
  }
  
  return flattenDictionary(dict);
}

// -----------------------------------------------------------------------------
void TextDictionaryBuilder::recursiveFlattenDictionary(const QString &context, QVector<TextDictionaryWord> &result, const TextDictionaryWord &scope) {
  if (scope.count < TEXT_MIN_OCCURRENCE) {
    return;
  }

  if (context.length() == TEXT_MIN_DICT_LENGTH) {
    TextDictionaryWord word;
    for (int i=0; i<TEXT_MIN_DICT_LENGTH; i++) {
      word.word.push_back(context[i]);
    }
    word.count = scope.count;
    word.indices = scope.indices;
    result.push_back(word);
  }

  QMapIterator<QChar, TextDictionaryWord> it(scope.children);
  while (it.hasNext()) {
    it.next();
    recursiveFlattenDictionary(context + it.key(), result, it.value());
  }
}

// -----------------------------------------------------------------------------
QVector<TextDictionaryWord> TextDictionaryBuilder::flattenDictionary(const TextDictionaryWord &dict) {
  QVector<TextDictionaryWord> result;
  recursiveFlattenDictionary("", result, dict);
  return result;
}

// -----------------------------------------------------------------------------
void TextDictionaryBuilder::addAllTextsFromRegistry(TextRegistry *reg) {
  for (TextMessage *msg : reg->messages()) {
    if (!msg->isCompressed()) {
      continue;
    }

    mFullTextBuffer += msg->textWithoutCommands() + "\0";
  }
}