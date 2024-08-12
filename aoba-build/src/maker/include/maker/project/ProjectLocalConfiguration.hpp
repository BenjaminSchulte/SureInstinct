#pragma once

#include <QSharedPointer>
#include <QStringList>
#include "../common/AobaZoom.hpp"

typedef QSharedPointer<class MakerProject> MakerProjectPtr;

class ProjectLocalConfiguration {
public:
  //! Constructor
  ProjectLocalConfiguration(const MakerProjectPtr &project) : mProject(project) {}

  //! Returns all assets
  inline const QStringList &loadedAssetIds() const { return mLoadedAssetIds; }

  //! Sets the loaded asset IDS
  inline void setLoadedAssetIds(const QStringList &list) { mLoadedAssetIds = list; }

  //! Returns the active asset id
  inline const QString &activeAssetId() const { return mActiveAssetId; }

  //! Sets the active asset id
  inline void setActiveAssetId(const QString &asset) { mActiveAssetId = asset; }

  //! Returns the zoom
  inline AobaZoom &zoom() { return mZoom; }

  //! Returns the zoom
  inline const AobaZoom &zoom() const { return mZoom; }

  //! Loads the configuration
  bool load();

  //! Saves the configuration
  void save();

private:
  //! Returns the filename
  QString fileName();

  //! List of all maps currently open
  QStringList mLoadedAssetIds;

  //! The active asset ID
  QString mActiveAssetId;

  //! The current zoom
  AobaZoom mZoom = AobaZoom(2.0f);

  //! The project
  MakerProjectPtr mProject;
};