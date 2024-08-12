#include <aoba/log/Log.hpp>
#include <QTextStream>
#include <QFile>
#include <maker/game/GameTranslationCsvAdapter.hpp>
#include <maker/game/GameProject.hpp>
#include <aoba/text/TextRegistry.hpp>
#include <aoba/text/TextTranslatableIndex.hpp>
#include <aoba/text/TextTranslatable.hpp>

using namespace Maker;
using namespace Aoba;

// -----------------------------------------------------------------------------
GameTranslationCsvAdapter::GameTranslationCsvAdapter(GameProject *project)
  : GameTranslationAdapter(project)
{
  mColumns = {ID, SOURCE_LANGUAGE, COMMENT, TARGET_LANGUAGE};
}

// -----------------------------------------------------------------------------
bool GameTranslationCsvAdapter::configure(const GameConfigNode &) {
  // TODO
  return true;
}

// -----------------------------------------------------------------------------
bool GameTranslationCsvAdapter::load() {
  return false;
}

// -----------------------------------------------------------------------------
bool GameTranslationCsvAdapter::save() {
  QStringList rows;
  rows << generateHeader();
  for (TextTranslatableIndexSection *section : mProject->texts()->translations()->sections()) {
    rows << generateRow(TextTranslatableIndexSectionItem(TextTranslatableIndexSectionItem::SECTION, section->id()));

    for (const TextTranslatableIndexSectionItem &item : section->items()) {
      QString row = generateRow(item);
      
      if (!row.isEmpty()) {
        rows.push_back(row);
      }
    }
  }

  QFile file(mProject->localeFile("translations.csv"));
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    mProject->log().error("Unable to open translation file: " + mProject->localeFile("translations.csv"));
    return false;
  }

  QTextStream out(&file);
  out << rows.join(mNewLine);

  return true;
}

// -----------------------------------------------------------------------------
QString GameTranslationCsvAdapter::generateHeader() const {
  QMap<QString, QString> map;
  map.insert("_id", "ID");
  map.insert("_section", "Section");
  map.insert("_comment", "Comment");

  for (const QString &language : mProject->texts()->translations()->languages()) {
    map.insert(language, language);
  }
  
  return buildRow(map);
}

// -----------------------------------------------------------------------------
QString GameTranslationCsvAdapter::generateRow(const Aoba::TextTranslatableIndexSectionItem &item) const {
  switch (item.mType) {
    case TextTranslatableIndexSectionItem::SECTION:
      if (mAddSectionHeader) {
        QMap<QString, QString> map;
        map.insert("_comment", "#### " + item.mText);
        return buildRow(map);
      }
      break;

    case TextTranslatableIndexSectionItem::HEADER:
      if (mAddSubSectionHeader) {
        QMap<QString, QString> map;
        map.insert("_comment", "## " + item.mText);
        return buildRow(map);
      }
      break;

    case TextTranslatableIndexSectionItem::COMMENT:
      if (mAddComment) {
        QMap<QString, QString> map;
        map.insert("_comment", item.mText);
        return buildRow(map);
      }
      break;

    case TextTranslatableIndexSectionItem::TRANSLATION: {
      QMap<QString, QString> map;
      map["_id"] = item.mTranslatable->symbolName();
      for (const QString &id : mProject->texts()->translations()->languages()) {
        map[id] = item.mTranslatable->getTranslation(mProject->texts()->translations()->addLanguage(id));
      }
      return buildRow(map);
    }
  }

  return "";
}

// -----------------------------------------------------------------------------
QString GameTranslationCsvAdapter::buildRow(const QMap<QString, QString> &properties) const {
  QStringList columns;
  QString inputLanguage = mProject->texts()->translations()->inputLanguageName();

  for (const auto &type : mColumns) {
    switch (type) {
      case ID: columns.push_back(escape(properties["_id"])); break;
      case SECTION: columns.push_back(escape(properties["_section"])); break;
      case COMMENT: columns.push_back(escape(properties["_comment"])); break;
      case SOURCE_LANGUAGE:
        columns.push_back(escape(properties[inputLanguage]));
        break;

      case TARGET_LANGUAGE:
        for (const QString &id : mProject->texts()->translations()->languages()) {
          if (id != inputLanguage) {
            columns.push_back(escape(properties[id]));
          }
        };
        break;
    }
  }

  return columns.join(mSeparator);
}

// -----------------------------------------------------------------------------
QString GameTranslationCsvAdapter::escape(const QString &column) const {
  if (column.indexOf(mSeparator) == -1
    && column.indexOf(mNewLine) == -1
    && column.indexOf(mQuote) == -1
  ) {
    return column;
  }

  return "TODO:::ESCAPE!!!";
}
