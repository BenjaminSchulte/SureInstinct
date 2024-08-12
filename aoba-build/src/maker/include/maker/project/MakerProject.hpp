#pragma once

#include <QString>
#include <QSharedPointer>
#include "MakerAssetTree.hpp"

namespace Maker {
class GameProject;
class GameLevelAsset;
}

typedef QSharedPointer<class MakerProject> MakerProjectPtr;

class MakerProject {
public:
  //! Constructor
  MakerProject(Maker::GameProject *project) : mProject(project) {}

  //! Deconstructor
  ~MakerProject();

  //! Loads a project
  static MakerProjectPtr load(const QString &folderName);

  //! The map tree
  inline MakerAssetTree &assetTree() { return mAssetTree; }

  //! Returns the game project
  inline Maker::GameProject *gameProject() const { return mProject; }

  //! Returns the game title
  QString title() const;

  //! Returns a level
  Maker::GameLevelAsset *getLevel(const QString &id) const;

private:
  //! The project
  Maker::GameProject *mProject;

  //! The asset tree
  MakerAssetTree mAssetTree;
};
