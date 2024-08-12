#include <QFile>
#include <aoba/log/Log.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameScripts.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameLinkResult.hpp>
#include <aoba/font/FontRegistry.hpp>
#include <aoba/font/Font.hpp>
#include <aoba/text/TextRegistry.hpp>
#include <aoba/text/TextTranslatable.hpp>
#include <aoba/text/TextTranslatableIndex.hpp>
#include <aoba/variable/RamVariableRegistry.hpp>
#include <aoba/variable/RamVariableSchema.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/linker/LinkerObjectSerializer.hpp>

using namespace Maker;
using namespace Aoba;

// -----------------------------------------------------------------------------
GameScripts::GameScripts(GameProject *project)
  : mProject(project)
  , mScriptCompiler(&mScriptProject)
{
  mScriptProject.setProjectInterface(this);
  mScriptProject.registerAssetResolver("Project", &mProjectAssetResolver);
}

// -----------------------------------------------------------------------------
bool GameScripts::initialize() {
  return mScriptCompiler.initialize();
}

// -----------------------------------------------------------------------------
bool GameScripts::load() {
  GameConfigReader config(GameConfigReader::fromFile(mProject, mProject->configFile("scripts.yml")));

  if (!getVariables()->schema()->restoreFromFile(mProject->objFile("variableschema.json"))) {
    mProject->log().note("Could not restore old variable schema file. Generating schema from scratch.");
  }

  for (const GameConfigNode &node : config["library"].asList()) {
    if (!loadScript(node.asString())) {
      return false;
    }
  }

  for (const GameConfigNode &node : config["init"].asList()) {
    addScript(node.asString());
  }

  for (const GameConfigNode &node : config["global"].asList()) {
    addGlobalScript(node["script"].asString(), node["symbol_name"].asString());
  }

  return true;
}

// -----------------------------------------------------------------------------
bool GameScripts::loadQueued() {
  QStringList queued(mScriptQueue);
  QStringList queuedFile(mScriptFileQueue);
  mScriptQueue.clear();
  mScriptFileQueue.clear();

  for (const QString &script : queued) {
    if (!loadScript(script)) {
      return false;
    }
  }
  for (const QString &script : queuedFile) {
    if (!loadScriptFile(script)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
QString GameScripts::getScriptSymbolName(const QString &func) const {
  return mScriptProject.getScriptSymbolName(func);
}

// -----------------------------------------------------------------------------
bool GameScripts::linkScripts(GameLinkResult &result) {
  QMapIterator<QString, QString> it(mGlobalScripts);
  while (it.hasNext()) {
    it.next();

    QString internalSymbol = getScriptSymbolName(it.key());
    if (internalSymbol.isEmpty()) {
      return false;
    }

    result.addCode("extern " + internalSymbol);
    result.addCode(it.value() + "=" + internalSymbol);
  }

  linkDataTables();

  if (!getVariables()->schema()->addFromScope(getVariables()->root())) {
    mProject->log().error("Failed to build game variables schema");
    return false;
  }
  if (!getVariables()->schema()->storeToFile(mProject->objFile("variableschema.json"))) {
    mProject->log().warning("Could not cache variables schema to disk");
  }
  if (!getVariables()->schema()->build(&mScriptProject)) {
    mProject->log().error("Failed to build game variables");
    return false;
  }
  result.addCode(getVariables()->schema()->getFmaCode());

  FMA::linker::LinkerObjectSerializer serializer(&mScriptProject.fmaLinkerObject());
  if (!serializer.serialize()) {
    return false;
  }

  QFile objectFile(mProject->objFile("scripts.fmaobj"));
  if (!objectFile.open(QIODevice::WriteOnly)) {
    return false;
  }

  objectFile.write((const char*)serializer.buffer().getData(), serializer.buffer().getSize());

  result.addObject(mProject->objFile("scripts.fmaobj"));

  return true;
}

// -----------------------------------------------------------------------------
void GameScripts::linkDataTables() {
  QMapIterator<QString, GameScriptDataTable> it(mDataTables);

  while (it.hasNext()) {
    it.next();

    FMA::linker::LinkerBlock *block = mScriptProject.fmaLinkerObject().createBlock();
    block->symbol(("__datatable_" + it.value().mId).toStdString());
    
    if (it.value().mValues.isEmpty()) {
      block->writeNumber(0, 1);
    } else {
      for (const auto &value : it.value().mValues) {
        block->writeNumber(value, it.value().mBytesPerRecord);
      }
    }
  }
}

// -----------------------------------------------------------------------------
bool GameScripts::loadScript(const QString &id) {
  auto task(mProject->log().assetTask("LOAD", "Script", id));

  if (!mScriptCompiler.parseAndInterpretFile(mProject->assetFile("scripts/" + id))) {
    return task.failed("Failed to load script");
  }

  task.succeeded();
  return true;
}

// -----------------------------------------------------------------------------
bool GameScripts::loadScriptFile(const QString &id) {
  auto task(mProject->log().assetTask("LOAD", "Script", id));

  if (!mScriptCompiler.parseAndInterpretFile(id)) {
    return task.failed("Failed to load script");
  }

  task.succeeded();
  return true;
}

// -----------------------------------------------------------------------------
void GameScripts::addTranslationHeader(const QString &text, const QString &context) {
  mProject->texts()->translations()->section(context)->header(text);
}

// -----------------------------------------------------------------------------
void GameScripts::addTranslationComment(const QString &text, const QString &context) {
  mProject->texts()->translations()->section(context)->comment(text);
}

// -----------------------------------------------------------------------------
QString GameScripts::addTranslation(const QString &text, const QString &fontName, const QString &context) {
  QStringList parts(fontName.split(':'));
  if (parts.size() != 2) {
    mProject->log().error("Font name must include font type: " + fontName);
    return "__missing_font";
  }

  FontSetInterface *fontSet = mProject->fonts()->set(parts[0]);
  if (!fontSet) {
    mProject->log().error("Font set is unknown: " + parts[0]);
    return "__missing_font";
  }
  
  FontInterface *font = fontSet->font(parts[1]);
  if (!font) {
    mProject->log().error("Font is unknown: " + parts[1]);
    return "__missing_font";
  }
  
  TextTranslatable *translatable = mProject->texts()->translations()->section(context)->text(text);
  translatable->addTranslation(mProject->texts()->translations()->inputLanguage(), text);
  translatable->addFont(font);
  return translatable->symbolName();
}

// -----------------------------------------------------------------------------
RamVariableRegistry *GameScripts::getVariables() {
  return mScriptProject.variables();
}

// -----------------------------------------------------------------------------
int GameScripts::getLocaleId(const QString &locale) {
  return mProject->texts()->translations()->addBuildLanguage(locale);
}

// -----------------------------------------------------------------------------
void GameScripts::lookupTable(const QString &type, const QString &id) {
  QString hash = type + "/" + id;
  uint8_t bytesPerRecord = 0;
  
  if (type == "uint8") { bytesPerRecord = 1; }
  else if (type == "uint16") { bytesPerRecord = 2; }
  else {
    return;
  }
  
  auto &table = mDataTables[hash];
  table.mId = id;
  table.mBytesPerRecord = bytesPerRecord;
}

// -----------------------------------------------------------------------------
int GameScripts::lookupTable(const QString &type, const QString &id, const QString &key, int value) {
  QString hash = type + "/" + id;
  uint8_t bytesPerRecord = 0;
  
  if (type == "uint8") { bytesPerRecord = 1; }
  else if (type == "uint16") { bytesPerRecord = 2; }
  else {
    return -1;
  }
  
  auto &table = mDataTables[hash];
  table.mId = id;
  table.mBytesPerRecord = bytesPerRecord;
  
  int index;
  if (!table.mRecords.contains(key)) {
    index = table.mValues.size();
    table.mValues.push_back(value);
    table.mRecords.insert(key, index);
  } else {
    index = table.mRecords[key];
  }

  return index;
}