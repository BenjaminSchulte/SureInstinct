#pragma once

#include <QMap>
#include <QString>

namespace Aoba {
class FontInterface;
class TextTranslatableIndex;

class TextTranslatable {
public:
  //! Constructor
  TextTranslatable(const QString &symbolName) : mSymbolName(symbolName) {}

  //! Adds a translation
  void addTranslation(int language, const QString &text);

  //! Adds a font
  void addFont(FontInterface *font);

  //! Returns a translation as raw text
  QString getTranslation(int language) const;

  //! Returns the ID
  inline QString symbolName() const { return mSymbolName; }

  //! Builds the text
  bool build(TextTranslatableIndex *index);

protected:
  //! The symbol
  QString mSymbolName;

  //! List of all translations
  QMap<int, QString> mTranslations;

  //! List of all fonts
  QVector<FontInterface*> mFonts;
};

}