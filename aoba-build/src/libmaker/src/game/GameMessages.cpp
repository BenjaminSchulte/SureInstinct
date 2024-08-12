#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameMessages.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameLinkResult.hpp>
#include <maker/game/GameTranslationCsvAdapter.hpp>
#include <aoba/asset/AssetLinkerObject.hpp>
#include <aoba/font/Font.hpp>
#include <aoba/font/FontLetterRegistry.hpp>
#include <aoba/font/FontRegistry.hpp>
#include <aoba/text/TextMessage.hpp>
#include <aoba/text/TextRegistry.hpp>
#include <aoba/text/TextTranslatableIndex.hpp>

using namespace Maker;
using namespace Aoba;

// -----------------------------------------------------------------------------
GameMessages::~GameMessages() {
  delete mInputAdapter;
  delete mOutputAdapter;
}

// -----------------------------------------------------------------------------
bool GameMessages::configure() {
  GameConfigReader config(GameConfigReader::fromFile(mProject, mProject->configFile("locales.yml")));

  mDictionaryBank = config["dictionary_bank"].asInt(0xD0);

  mProject->texts()->translations()->setInputLanguage(config["input_locale"].asString("en"));
  for (const auto &item : config["all_locales"].asList()) {
    mProject->texts()->translations()->addLanguage(item.asString("en"));
  }

  for (const auto &item : config["build_locales"].asList()) {
    mProject->texts()->translations()->addBuildLanguage(item.asString("en"));
  }

  for (const auto &item : config["adapters"].asList()) {
    createAdapter(item);
  }

  return true;
}

// -----------------------------------------------------------------------------
void GameMessages::createAdapter(const GameConfigNode &config) {
  GameTranslationAdapter *adapter = nullptr;
  QString type = config["adapter"].asString("");

  if (type == "csv") {
    adapter = new GameTranslationCsvAdapter(mProject);
  } else {
    mProject->log().warning("Unable to create adapter of type " + type);
    return;
  }

  if (!adapter->configure(config)) {
    mProject->log().warning("Could not configure adapter of type " + type);
    delete adapter;
    return;
  }

  if (config["input"].asBool()) {
    delete mInputAdapter;
    mInputAdapter = adapter;
  } else if (config["output"].asBool()) {
    delete mOutputAdapter;
    mOutputAdapter = adapter;
  } else {
    delete mInputAdapter;
    mInputAdapter = adapter;
  }
}

// -----------------------------------------------------------------------------
bool GameMessages::load() {
  GameConfigReader config(GameConfigReader::fromFile(mProject, mProject->configFile("global_messages.yml")));

  for (const QString &key : config["all_fonts"].keys()) {
    TextMessage *message = createMessage(config["all_fonts"][key], key);

    for (FontSetInterface *fontSet : mProject->fonts()->allSets()) {
      for (FontInterface *font : fontSet->allFonts()) {
        message->useInFont(font);
      }
    }

    mGlobalMessages.insert(key, message);
  }

  return true;
}

// -----------------------------------------------------------------------------
bool GameMessages::build() {
  {
    auto task(mProject->log().assetTask("BUILD", "Text", "TextTranslations"));
    if (!mProject->texts()->buildTranslations()) {
      return false;
    }
  }
  {
    auto task(mProject->log().assetTask("BUILD", "Text", "FontLetterIndex"));
    mProject->fonts()->letters().buildIndex(mProject->fonts());
  }
  {
    auto task(mProject->log().assetTask("BUILD", "Text", "TextDictionary"));

    // TODO: use configurable bank for dictionary
    if (!mProject->texts()->build(&mProject->fonts()->letters(), mDictionaryBank)) {
      return false;
    }
  }
  
  return true;
}

// -----------------------------------------------------------------------------
bool GameMessages::linkMessages(GameLinkResult &result) {
  for (const QString &obj : mProject->texts()->linkerObject()->getFmaObjectFiles()) {
    result.addObject(obj);
  }

  result.addCode("module SystemText");

  QMapIterator<QString, TextMessage*> it(mGlobalMessages);
  while (it.hasNext()) {
    it.next();
    result.addCode("extern " + it.value()->globalReference());
    result.addCode(it.key() + "=" + it.value()->globalReference());
  }

  result.addCode("end");

  return true;
}

// -----------------------------------------------------------------------------
TextMessage *GameMessages::createMessage(const GameConfigNode &node, const QString &key) {
  QString text = node["message"].asString("");
  bool compress = node["compress"].asBool(true);
  bool important = node["important"].asBool(false);

  TextMessage *message = mProject->texts()->createMessage("global__" + key, compress);
  if (important) {
    message->setIsImportant();
  }

  message->addScript(text);
  return message;
}

// -----------------------------------------------------------------------------
bool GameMessages::loadTranslations() const {
  if (!mInputAdapter) {
    return false;
  }

  return mInputAdapter->load();
}

// -----------------------------------------------------------------------------
bool GameMessages::saveTranslations() const {
  if (!mInputAdapter && !mOutputAdapter) {
    return false;
  }

  GameTranslationAdapter *adapter = mOutputAdapter;
  if (!adapter) {
    adapter = mInputAdapter;
  }

  return adapter->save();
}

// -----------------------------------------------------------------------------
