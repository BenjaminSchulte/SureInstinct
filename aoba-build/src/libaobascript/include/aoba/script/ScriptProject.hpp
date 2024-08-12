#pragma once

#include <QString>
#include <QMap>
#include <fma/Project.hpp>
#include <fma/linker/LinkerObject.hpp>
#include "./ScriptMemorySymbolMap.hpp"

namespace Aoba {
class ScriptAssetResolver;
class ScriptProjectInterface;
class RamVariableRegistry;

class ScriptProject {
public:
  //! Constructor
  ScriptProject();

  //! Deconstructor
  ~ScriptProject();
  
  //! Returns the FMA project
  inline FMA::Project &fmaProject() { return mProject; }

  //! Returns the linker object
  inline FMA::linker::LinkerObject &fmaLinkerObject() { return mLinkerObject; }

  //! Returns the script symbol name for a script
  QString getScriptSymbolName(const QString &func) const;

  //! Returns the variable registry
  inline RamVariableRegistry *variables() const { return mVariables; }

  //! Registers an asset type resolver
  inline void registerAssetResolver(const QString &id, ScriptAssetResolver *resolver) {
    mAssetResolvers.insert(id, resolver);
  }

  //! Returns whether an asset resolver exists
  inline bool hasAssetResolver(const QString &id) const {
    return mAssetResolvers.contains(id);
  }

  //! Returns the asset resolver
  inline ScriptAssetResolver *assetResolver(const QString &id) const {
    return mAssetResolvers[id];
  }

  //! Sets the project interface
  void setProjectInterface(ScriptProjectInterface *i) { mInterface = i; }

  //! Returns the project interface
  inline ScriptProjectInterface *projectInterface() const { return mInterface; }

  //! Returns the symbol map
  inline ScriptMemorySymbolMap *symbolMap() { return &mSymbolMap; }

protected:
  //! The FMA project
  FMA::Project mProject;

  //! The variable registry
  RamVariableRegistry *mVariables;

  //! The symbol map
  ScriptMemorySymbolMap mSymbolMap;

  //! A linker object
  FMA::linker::LinkerObject mLinkerObject;

  //! Registers the asset type resolvers
  QMap<QString, ScriptAssetResolver*> mAssetResolvers;

  //! The interface
  ScriptProjectInterface *mInterface = nullptr;
};

}