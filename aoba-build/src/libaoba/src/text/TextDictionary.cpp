#include <aoba/log/Log.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <aoba/asset/AssetLinkerObject.hpp>
#include <aoba/text/TextProperties.hpp>
#include <aoba/text/TextRegistry.hpp>
#include <aoba/text/TextDictionary.hpp>
#include <aoba/font/FontLetterRegistry.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
QString TextDictionaryWord::asString() const {
  QString result;
  for (const QChar &c : word) {
    result += c;
  }
  return result;
}

// -----------------------------------------------------------------------------
void TextDictionary::writeEndOfText(FMA::linker::LinkerBlock *block) const {
  uint8_t eot = TEXT_CMD_END_OF_TEXT;
  block->write(&eot, 1);
}

// -----------------------------------------------------------------------------
void TextDictionary::writeChar(FMA::linker::LinkerBlock *block, QChar c, FontLetterRegistry *letters) const {
  int id = letters->indexOf(c);
  if (id == -1) {
    Aoba::log::warn("Could not find character: " + QString(c));
  }

  if (id < TEXT_NUM_ONE_BYTE_LETTERS) {
    block->write(&id, 1);
  } else if (id >= TEXT_MAX_NUM_LETTERS) {
    Aoba::log::warn("Engine does only support up to " + QString::number(TEXT_MAX_NUM_LETTERS) + " different letters.");
  } else {
    block->writeNumber(((id >> 8) & 0xF) | 0xC0, 1);
    block->writeNumber(id, 1);
  }
}

// -----------------------------------------------------------------------------
void TextDictionary::writeReference(FMA::linker::LinkerBlock *block, int ref) const {
  if (ref >= 0x4000) {
    Aoba::log::warn("Dictionary item is out of range");
    return;
  }

  block->writeNumber(((ref >> 8) & 0xF) | 0xD0, 1);
  block->writeNumber(ref, 1);
}

// -----------------------------------------------------------------------------
void TextDictionary::write(FMA::linker::LinkerBlock *block, const QString &text, FontLetterRegistry *letters, bool compressed) const {
  int textLength = text.length();
  int dictionaryItem;

  for (int i=0; i<textLength; i++) {
    if (compressed && searchDictionary(text, &dictionaryItem, i)) {
      writeReference(block, dictionaryItem);
    } else {
      writeChar(block, text[i], letters);
    }
  }
}

// -----------------------------------------------------------------------------
bool TextDictionary::searchDictionary(const QString &text, int *result, int &startAt) const {
  int textLength = text.length();
  bool success = false;
  const TextDictionaryIndex *index = &mIndex;
  int offset = startAt;

  while (offset < textLength) {
    QChar letter = text[offset++];
    
    if (!index->children.contains(letter)) {
      break;
    }
    index = index->children[letter];

    if (index->index >= 0) {
      success = true;
      startAt = offset - 1;
      *result = index->index;
    }
  }

  return success;
}

// -----------------------------------------------------------------------------
void TextDictionary::setTextDictionary(const QVector<TextDictionaryWord> &dict) {
  for (const auto &item : dict) {
    addWord(item.word);
  }
}

// -----------------------------------------------------------------------------
void TextDictionary::addWord(const QVector<QChar> &word) {
  int index = mWords.length();
  mWords.push_back(word);

  TextDictionaryIndex *scope = &mIndex;
  for (const QChar &letter : word) {
    if (!scope->children.contains(letter)) {
      scope->children.insert(letter, new TextDictionaryIndex());
    }
    scope = scope->children[letter];
  }

  scope->index = index;
}

// -----------------------------------------------------------------------------
bool TextDictionary::build(TextRegistry *reg, FontLetterRegistry *letters, uint8_t bank) const {
  FMA::linker::LinkerBlock *index = reg->linkerObject()->createLinkerBlock("__asset_dictionary", bank);
  FMA::linker::LinkerBlock *data = reg->linkerObject()->createLinkerBlock("__asset_dictionary_data", bank);

  if (mWords.isEmpty()) {
    // Prevent errors when no dictionary is generated
    writeEndOfText(index);
    writeEndOfText(data);
    return true;
  }

  for (const QVector<QChar> &word : mWords) {
    index->writeNumber(data->getSize(), 2);

    for (const QChar &l : word) {
      writeChar(data, l, letters);
    }
    writeEndOfText(data);
  }

  return true;
}
