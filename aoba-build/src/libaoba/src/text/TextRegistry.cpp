#include <aoba/log/Log.hpp>
#include <aoba/asset/AssetLinkerObject.hpp>
#include <aoba/text/TextMessage.hpp>
#include <aoba/text/TextRegistry.hpp>
#include <aoba/text/TextDictionary.hpp>
#include <aoba/text/TextDictionaryBuilder.hpp>
#include <aoba/text/TextTranslatableIndex.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
TextRegistry::TextRegistry(Project *project)
  : mProject(project)
  , mTranslations(new TextTranslatableIndex(this))
  , mLinkerObject(new AssetLinkerObject("textregistry", mProject))
{}

// -----------------------------------------------------------------------------
TextRegistry::~TextRegistry() {
  for (TextMessage *msg : mMessages) {
    delete msg;
  }

  delete mDictionary;
  delete mLinkerObject;
  delete mTranslations;
}

// -----------------------------------------------------------------------------
bool TextRegistry::build(FontLetterRegistry *letters, uint8_t dictionaryBank) {
  mDictionary = new TextDictionary();

  TextDictionaryBuilder builder(mDictionary, letters);
  if (!builder.build(this)) {
    return false;
  }

  for (TextMessage *msg : mMessages) {
    if (!msg->build(letters)) {
      return false;
    }
  }

  if (!mDictionary->build(this, letters, dictionaryBank)) {
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TextRegistry::buildTranslations() {
  return mTranslations->build();
}

// -----------------------------------------------------------------------------
TextMessage *TextRegistry::createMessage(const QString &uniqueIdentifier, bool compressed) {
  QString identifierToUse = uniqueIdentifier;

  if (mMessages.contains(uniqueIdentifier)) {
    int number = 2;

    while (mMessages.contains(uniqueIdentifier + QString::number(number))) {
      number++;
    }

    identifierToUse = uniqueIdentifier + QString::number(number);
    Aoba::log::warn("Text Registry already contains a text with the identifier " + uniqueIdentifier + ". Using " + identifierToUse + " instead.");
  }

  TextMessage *msg = new TextMessage(this, identifierToUse, compressed);
  mMessages.insert(identifierToUse, msg);
  return msg;
}