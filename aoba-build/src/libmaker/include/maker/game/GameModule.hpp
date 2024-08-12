#pragma once

#include "GameModuleHeader.hpp"
#include "GameModuleProperties.hpp"

namespace Maker {

class GameModuleList;
class GameProject;

class GameModule {
public:
  //! Constructor
  GameModule(GameModuleList *list, const GameModuleHeader &header)
    : mModuleList(list)
    , mHeader(header)
  {}

  //! Returns the header
  inline const GameModuleHeader &header() const { return mHeader; }

  //! Loads the module
  bool load();

  //! Returns fma code
  QString buildFmaCode(GameProject *project) const;

  //! Returns all dependencies
  inline const QVector<GameModule*> &dependencies() const { return mDependendModules; }

  //! All modules including this one
  inline QVector<GameModule*> allModules() {
    QVector<GameModule*> parent(dependencies());
    parent.push_back(this);
    return parent;
  }

  //! Returns the properties
  inline const GameModuleProperties &properties() const { return mProperties; }

private:
  //! Loads a dependency
  bool loadDependency(const GameModuleDependency &);

  //! List of all game modules
  GameModuleList *mModuleList;

  //! The header
  GameModuleHeader mHeader;

  //! Properties defined by this module
  GameModuleProperties mProperties;

  //! List of dependend modules in order
  QVector<GameModule*> mDependendModules;

  //! List of all modules already added
  QStringList mLoadedDependendModules;
};

}