#pragma once

#include <QDir>
#include <QMap>
#include <QVector>

namespace Maker {
class GameModuleHeader;
struct GameModuleDependency;

class GameModuleRegistry {
public:
  //! Constructor
  GameModuleRegistry(GameModuleRegistry *parent = nullptr);

  //! Deconstructor
  ~GameModuleRegistry();

  //! Unloads all modules
  void unloadAll();

  //! Finds all modules using the environment variables
  void findModulesFromEnvironmentVariables();

  //! Returns a module header
  const GameModuleHeader *find(const GameModuleDependency &) const;

  //! Searches for modulest
  void findModules(const QDir &);

  //! Loads a module
  bool loadModuleHeader(const QDir &);

  //! Returns whether the module header is loaded
  inline bool hasModuleHeaderLoaded(const QDir &dir) const {
    return mModuleByPath.contains(dir.absolutePath()) || (mParent && mParent->hasModuleHeaderLoaded(dir));
  }

  //! Returns a module by its path
  const GameModuleHeader *moduleByPath(const QDir &path) const {
    return mModuleByPath[path.absolutePath()];
  }

private:
  //! Parent module registry
  GameModuleRegistry *mParent;

  //! List of loaded modules
  QMap<QString, QVector<GameModuleHeader*>> mModuleHeaders;
  
  //! List of modules by its path
  QMap<QString, GameModuleHeader*> mModuleByPath;
};

}