#pragma once

#include <QString>
#include <QVector>
#include "GameTranslationAdapter.hpp"

namespace Aoba {
  struct TextTranslatableIndexSectionItem;
}

namespace Maker {

class GameTranslationCsvAdapter : public GameTranslationAdapter {
public:
  //! Column types
  enum Column {
    ID,
    SECTION,
    COMMENT,
    SOURCE_LANGUAGE,
    TARGET_LANGUAGE
  };

  //! Constructor
  GameTranslationCsvAdapter(GameProject *project);

  //! Configures the adapter
  bool configure(const GameConfigNode &node) override;

  //! Loads the translations
  bool load() override;

  //! Saves the translations
  bool save() override;

private:
  //! Generates the header
  QString generateHeader() const;

  //! Generates a row
  QString generateRow(const Aoba::TextTranslatableIndexSectionItem &item) const;

  //! Builds a row
  QString buildRow(const QMap<QString, QString> &properties) const;

  //! Escapes a row
  QString escape(const QString &column) const;
  
  //! The separator
  QString mSeparator = ";";

  //! The quote
  QString mQuote = "\"";

  //! The newline
  QString mNewLine = "\n";

  //! List of all columns to add
  QVector<Column> mColumns;

  //! Whether to add full line sections
  bool mAddSectionHeader = true;

  //! Whether to add full line sections
  bool mAddSubSectionHeader = true;

  //! Whether to add full line sections
  bool mAddComment = true;
};

}