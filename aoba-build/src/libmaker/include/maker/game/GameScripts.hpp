#pragma once

#include <aoba/script/ScriptProject.hpp>
#include <aoba/script/ScriptCompiler.hpp>
#include <aoba/script/ScriptProjectInterface.hpp>
#include <aoba/project/ProjectScriptAssetResolver.hpp>

namespace Maker {
class GameProject;

struct GameScriptDataTable {
  QString mId;
  uint8_t mBytesPerRecord;
  QVector<int> mValues;
  QMap<QString, int> mRecords;
};

class GameScripts : public Aoba::ScriptProjectInterface {
public:
  //! Constructor
  GameScripts(GameProject *project);

  //! Initializes the module
  bool initialize();

  //! Loads the config
  bool load();

  //! Loads queued scripts
  bool loadQueued();

  //! Loads a script
  bool loadScript(const QString &id);

  //! Loads a script
  bool loadScriptFile(const QString &id);

  //! Links scripts
  bool linkScripts(GameLinkResult &result);

  //! Returns the script project
  inline Aoba::ScriptProject &scriptProject() { return mScriptProject; }

  //! Returns the script compiler
  inline Aoba::ScriptCompiler &scriptCompiler() { return mScriptCompiler; }

  //! Adds a script to the queue
  inline void addScript(const QString &id) { mScriptQueue.push_back(id); }

  //! Adds a script to the queue
  inline void addScriptFile(const QString &id) { mScriptFileQueue.push_back(id); }

  //! Adds a global script
  inline void addGlobalScript(const QString &script, const QString &symbol) {
    mGlobalScripts.insert(script, symbol);
  }

  //! Returns the script symbol name for a script
  QString getScriptSymbolName(const QString &func) const;

  //! Adds a translation
  void addTranslationHeader(const QString &text, const QString &context) override;

  //! Adds a translation
  void addTranslationComment(const QString &text, const QString &context) override;

  //! Adds a translation
  QString addTranslation(const QString &text, const QString &font, const QString &context) override;

  //! Returns the language ID
  int getLocaleId(const QString &locale) override;

  //! Looks up a generated table
  void lookupTable(const QString &type, const QString &id) override;

  //! Looks up a generated table
  int lookupTable(const QString &type, const QString &id, const QString &key, int value) override;

  //! Returns the variable registry
  Aoba::RamVariableRegistry *getVariables() override;

protected:
  //! Links all data tables
  void linkDataTables();

  //! The project
  GameProject *mProject;

  //! The script project
  Aoba::ScriptProject mScriptProject;

  //! The script compiler
  Aoba::ScriptCompiler mScriptCompiler;

  //! The script resolver
  Aoba::ProjectScriptAssetResolver mProjectAssetResolver;

  //! List of initialize scripts
  QStringList mScriptQueue;

  //! List of initialize scripts
  QStringList mScriptFileQueue;

  //! Map of global scripts
  QMap<QString, QString> mGlobalScripts;

  //! List of lookup tables
  QMap<QString, GameScriptDataTable> mDataTables;
};

}