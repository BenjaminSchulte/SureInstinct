#pragma once

#include <QObject>
#include <QSharedPointer>
#include <maker/game/GameModuleRegistry.hpp>
#include "project/ProjectLocalConfiguration.hpp"

typedef QSharedPointer<class MakerProject> MakerProjectPtr;

class TileRenderCache;

class GlobalContext : public QObject {
  Q_OBJECT

public:
  //! Constructor
  GlobalContext();

  //! Returns the singleton
  static GlobalContext *get();

  //! Returns the project
  inline MakerProjectPtr project() const { return mProject; }

  //! Sets the project
  void setProject(const MakerProjectPtr &project);

  //! Returns the tile render cache
  inline TileRenderCache *tileRenderCache() const { return mTileRenderCache; }

  //! Returns the local configuration
  inline ProjectLocalConfiguration &localConfig() { return mProjectLocalConfiguration; }

  //! Returns the registry
  inline Maker::GameModuleRegistry *moduleRegistry() { return &mModuleRegistry; }

private:
  //! The maker project
  MakerProjectPtr mProject;

  //! The singleton instance
  static GlobalContext* mGlobalInstance;

  //! The tileset render cache
  TileRenderCache *mTileRenderCache;
  
  //! The local configuration
  ProjectLocalConfiguration mProjectLocalConfiguration;

  //! The module registry
  Maker::GameModuleRegistry mModuleRegistry;
};

GlobalContext &context();