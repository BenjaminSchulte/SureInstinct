#pragma once

#include <QString>
#include <QMap>
#include <QVector>

namespace Aoba {
class TextTranslatable;
class TextRegistry;
class FontInterface;

struct TextTranslatableIndexSectionItem {
  enum Type {
    SECTION,
    HEADER,
    COMMENT,
    TRANSLATION
  };

  TextTranslatableIndexSectionItem() {}
  TextTranslatableIndexSectionItem(Type type, const QString &text) : mType(type), mText(text) {}
  TextTranslatableIndexSectionItem(TextTranslatable *text) : mType(TRANSLATION), mTranslatable(text) {}

  Type mType;
  QString mText;
  TextTranslatable *mTranslatable;
};

class TextTranslatableIndex;

class TextTranslatableIndexSection {
public:
  //! Constructor
  TextTranslatableIndexSection(TextTranslatableIndex *owner, const QString &section)
    : mOwner(owner)
    , mSection(section)
  {}

  //! Deconstructor
  ~TextTranslatableIndexSection();

  //! Returns the section name
  inline const QString &id() const { return mSection; }

  //! Adds a text
  TextTranslatable *text(const QString &text);

  //! Adds a section header
  void header(const QString &text);

  //! Adds a comment
  void comment(const QString &text);

  //! Returns a hash
  static QString textHash(const QString &text, const QString &section);

  //! Returns all items
  inline const QVector<TextTranslatableIndexSectionItem> &items() const { return mItems; }

  //! Builds the section
  bool build();

protected:
  //! The owner
  TextTranslatableIndex *mOwner;

  //! The section
  QString mSection;

  //! All records
  QVector<TextTranslatableIndexSectionItem> mItems;

  //! Map of all texts
  QMap<QString, TextTranslatable*> mTranslations;
};

class TextTranslatableIndex {
public:
  //! Constructor
  TextTranslatableIndex(TextRegistry *owner) : mRegistry(owner) {}

  //! Deconstructor
  ~TextTranslatableIndex();

  //! Returns a section
  TextTranslatableIndexSection *section(const QString &section);

  //! Overrides the default language
  void setInputLanguage(const QString &language);

  //! Returns the default language
  inline int inputLanguage() const { return mInputLanguage; }

  //! Adds a new language
  int addLanguage(const QString &language);

  //! Adds a new language for building
  int addBuildLanguage(const QString &language);

  //! Generates a hash
  static QString sectionHash(const QString &section);

  //! Returns the input language as String
  inline QString inputLanguageName() const { return mLanguages[mInputLanguage]; }

  //! Returns all languages
  inline QList<TextTranslatableIndexSection *> sections() const { return mSections.values(); }

  //! Returns all languages
  inline const QVector<QString> &languages() const { return mLanguages; }

  //! Returns all languages
  inline const QVector<QString> &buildLanguages() const { return mBuildLanguages; }

  //! Builds the translations
  bool build();

  //! Returns the text registry
  inline TextRegistry *textRegistry() const { return mRegistry; }

  //! Returns whether the index has been built
  inline bool hasBeenBuilt() const { return mHasBeenBuilt; }

protected:
  //! The registry
  TextRegistry *mRegistry;

  //! The language of all untranslated texts
  int mInputLanguage = -1;

  //! Map of all sections
  QMap<QString, TextTranslatableIndexSection*> mSections;

  //! List of all languages
  QVector<QString> mLanguages;

  //! List of all languages
  QVector<QString> mBuildLanguages;

  //! Whether the translations have already been built
  bool mHasBeenBuilt = false;
};

}