#include <QCryptographicHash>
#include <aoba/log/Log.hpp>
#include <aoba/text/TextTranslatableIndex.hpp>
#include <aoba/text/TextTranslatable.hpp>
#include <aoba/font/Font.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
TextTranslatableIndexSection::~TextTranslatableIndexSection() {
  for (TextTranslatable *text : mTranslations) {
    delete text;
  }
}

// -----------------------------------------------------------------------------
TextTranslatable *TextTranslatableIndexSection::text(const QString &message) {
  if (mOwner->hasBeenBuilt()) {
    Aoba::log::error("Tried to add a translatable text to an index which has been already built.");
  }

  QString hash = textHash(message, mSection);
  if (mTranslations.contains(hash)) {
    return mTranslations[hash];
  }

  TextTranslatable *text = new TextTranslatable("__asset_translation_" + hash);
  mItems.push_back(text);
  mTranslations.insert(hash, text);
  return text;
}

// -----------------------------------------------------------------------------
void TextTranslatableIndexSection::header(const QString &text) {
  mItems.push_back(TextTranslatableIndexSectionItem(TextTranslatableIndexSectionItem::HEADER, text));
}

// -----------------------------------------------------------------------------
void TextTranslatableIndexSection::comment(const QString &text) {
  mItems.push_back(TextTranslatableIndexSectionItem(TextTranslatableIndexSectionItem::COMMENT, text));
}

// -----------------------------------------------------------------------------
QString TextTranslatableIndexSection::textHash(const QString &text, const QString &section) {
  QString data(text + "@" + TextTranslatableIndex::sectionHash(section));
  return QCryptographicHash::hash(data.toLatin1(), QCryptographicHash::Md5).toHex();
}

// -----------------------------------------------------------------------------
bool TextTranslatableIndexSection::build() {  
  for (TextTranslatable *text : mTranslations) {
    if (!text->build(mOwner)) {
      return false;
    }
  }
  return true;
}



// -----------------------------------------------------------------------------
TextTranslatableIndex::~TextTranslatableIndex() {
  for (TextTranslatableIndexSection *section : mSections) {
    delete section;
  }
}

// -----------------------------------------------------------------------------
TextTranslatableIndexSection *TextTranslatableIndex::section(const QString &section) {
  QString id = sectionHash(section);

  if (!mSections.contains(id)) {
    mSections.insert(id, new TextTranslatableIndexSection(this, section));
  }

  return mSections[id];
}

// -----------------------------------------------------------------------------
void TextTranslatableIndex::setInputLanguage(const QString &language) {
  mInputLanguage = addLanguage(language);
}

// -----------------------------------------------------------------------------
int TextTranslatableIndex::addLanguage(const QString &language) {
  int index = mLanguages.indexOf(language);
  if (index == -1) {
    index = mLanguages.size();
    mLanguages.push_back(language);
  }
  return index;
}

// -----------------------------------------------------------------------------
int TextTranslatableIndex::addBuildLanguage(const QString &language) {
  int index = mBuildLanguages.indexOf(language);
  if (index == -1) {
    index = mBuildLanguages.size();
    mBuildLanguages.push_back(language);
  }
  return index;
}

// -----------------------------------------------------------------------------
QString TextTranslatableIndex::sectionHash(const QString &section) {
  return QCryptographicHash::hash(section.toLatin1(), QCryptographicHash::Md5).toHex();
}

// -----------------------------------------------------------------------------
bool TextTranslatableIndex::build() {
  mHasBeenBuilt = true;

  for (TextTranslatableIndexSection *section : mSections) {
    if (!section->build()) {
      return false;
    }
  }

  return true;
}