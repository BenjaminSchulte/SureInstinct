#include <aoba/log/Log.hpp>
#include <QCryptographicHash>
#include <aoba/asset/AssetLinkerObject.hpp>
#include <aoba/text/TextTranslatable.hpp>
#include <aoba/text/TextTranslatableIndex.hpp>
#include <aoba/text/TextRegistry.hpp>
#include <aoba/text/TextMessage.hpp>
#include <aoba/font/Font.hpp>
#include <fma/linker/LinkerBlock.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
void TextTranslatable::addTranslation(int language, const QString &text) {
  mTranslations.insert(language, text);
}

// -----------------------------------------------------------------------------
void TextTranslatable::addFont(FontInterface *font) {
  mFonts.append(font);
}

// -----------------------------------------------------------------------------
QString TextTranslatable::getTranslation(int language) const {
  return mTranslations[language];
}

// -----------------------------------------------------------------------------
bool TextTranslatable::build(TextTranslatableIndex *index) {
  FMA::linker::LinkerBlock *block = index->textRegistry()->linkerObject()->createLinkerBlock(mSymbolName);

  QString defaultText = mTranslations.first();
  if (mTranslations.contains(index->inputLanguage())) {
    defaultText = mTranslations[index->inputLanguage()];
  }

  for (const QString &language : index->buildLanguages()) {
    int languageId = index->addLanguage(language);
    QString text = defaultText;

    if (mTranslations.contains(languageId)) {
      text = mTranslations[languageId];
    } else {
      Aoba::log::warn("Missing translation for " + mSymbolName);
    }

    QString hash(QCryptographicHash::hash(text.toLatin1(), QCryptographicHash::Md5).toHex());
    TextMessage *message = index->textRegistry()->message(hash);
    if (!message) {
      message = index->textRegistry()->createMessage(hash);
      message->addScript(text);
    }

    for (FontInterface *font : mFonts) {
      message->useInFont(font);
    }

    block->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(message->globalReference().toStdString())), 3);
  }

  return true;
}
