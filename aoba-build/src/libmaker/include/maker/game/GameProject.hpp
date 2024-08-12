#pragma once

#include <QDir>
#include <aoba/project/Project.hpp>
#include <maker/game/GameModule.hpp>
#include <maker/game/GameModuleList.hpp>
#include <maker/game/GameModuleRegistry.hpp>

#include "GameAssets.hpp"
#include "GameScripts.hpp"
#include "GameLog.hpp"
#include "GameAssetTypes.hpp"
#include "GameMessages.hpp"
#include "GameBuildTarget.hpp"

namespace Aoba {
class AbstractAssetSet;
}

namespace Maker {

class GameProject : public Aoba::Project {
public:
  //! Constructor
  GameProject(const QDir &path, GameModuleRegistry *moduleRegistry);

  //! Deconstructor
  ~GameProject();

  //! Initiliaze
  bool initialize();

  //! Loads all assets
  bool load();

  //! Builds all assets
  bool build();

  //! Links
  bool link();

  //! Returns the project
  inline Aoba::Project *project() { return this; }

  //! Returns the asset
  inline const QDir &path() const { return mPath; }

  //! Returns the asset
  inline GameScripts &scripts() { return mScripts; }

  //! Returns the asset
  inline GameAssets &assets() { return mAssets; }

  //! Returns the messages
  inline GameMessages &messages() { return mMessages; }

  //! Returns the asset types
  inline GameAssetTypes &assetTypes() { return mAssetTypes; }

  //! Returns the build target
  inline GameBuildTarget &buildTarget() { return mBuildTarget; }

  //! Returns the log
  inline const GameLog &log() const { return mLog; }

  //! Returns the asset path
  inline QDir configPath() const { return QDir(mPath.absoluteFilePath("config")); }

  //! Returns the asset path
  inline QString configFile(const QString &file) const { return configPath().absoluteFilePath(file); }

  //! Returns the asset path
  inline QDir assetPath() const { return QDir(mPath.absoluteFilePath("assets")); }

  //! Returns the asset path
  QString relativeAssetFile(const QString &) const;

  //! Returns the asset path
  inline QString assetFile(const QString &type) const { return assetPath().absoluteFilePath(type); }

  //! Returns the obj path
  inline QDir objPath() const { return QDir(mPath.absoluteFilePath("obj")); }

  //! Returns the asset path
  QString objFile(const QString &type) const override { return objPath().absoluteFilePath(type); }

  //! Returns the obj path
  inline QDir localePath() const { return QDir(mPath.absoluteFilePath("locale")); }

  //! Returns the asset path
  QString localeFile(const QString &type) const { return localePath().absoluteFilePath(type); }

  //! Looks up a generated table
  void lookupTable(const QString &type, const QString &id) override;

  //! Looks up a generated table
  int lookupTable(const QString &type, const QString &id, const QString &key, int value) override;

  //! Returns an asset type set
  Aoba::AbstractAssetSet *assetSet(const QString &id) const override {
    return mAssetTypes.getAbstract(id)->assetSet();
  };

  //! Returns the internal script symbol for a script
  QString getScriptSymbolName(const QString &func) const override;

  //! Returns the module
  inline GameModule *gameModule() const { return mModule; }

protected:
  //! Loads the default module properties
  void loadModuleProperties();

  //! Returns the current GIT commit
  QString gitCommit() const;

  //! Links the project version to the game
  void linkProjectVersion(GameLinkResult &result);

  //! Links the asset lists
  void linkAssetLists(const QString &id, GameLinkResult &result, Aoba::AssetByteListLinker *list);

  //! The project path
  QDir mPath;

  //! The project module
  GameModule *mModule = nullptr;

  //! The module resolver
  GameModuleRegistry mModuleRegistry;

  //! List of loaded modules
  GameModuleList mLoadedModules;

  //! The build target
  GameBuildTarget mBuildTarget;

  //! The assets
  GameScripts mScripts;

  //! The assets
  GameAssets mAssets;

  //! The asset types
  GameAssetTypes mAssetTypes;

  //! The asset types
  GameMessages mMessages;

  //! The log
  GameLog mLog;
};

}
