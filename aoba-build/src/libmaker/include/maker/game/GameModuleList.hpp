#pragma once

#include <QMap>

namespace Maker {
class GameModuleRegistry;
class GameModuleHeader;
class GameModule;
struct GameModuleDependency;

class GameModuleList {
public:
  //! Constructor
  GameModuleList(GameModuleRegistry *reg) : mRegistry(reg) {}

  //! Deconstructor
  ~GameModuleList();

  //! Deletes all modules
  void deleteAll();

  //! Loads a module
  GameModule *load(const QDir &);

  //! Loads a header
  GameModule *load(const GameModuleHeader &);

  //! Loads a module
  GameModule *load(const GameModuleDependency &);

private:
  //! Registry
  GameModuleRegistry *mRegistry;

  //! List of all modules
  QMap<QString, GameModule*> mModules;
};

}