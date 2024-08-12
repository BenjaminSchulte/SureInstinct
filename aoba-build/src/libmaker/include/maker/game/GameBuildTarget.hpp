#pragma once

#include <QStringList>

namespace Maker {
class GameProject;
class GameConfigNode;

class GameBuildTarget {
public:
  //! Constructor
  GameBuildTarget(GameProject *project) : mProject(project) {}
  
  //! Initializes the build target
  bool initialize();

  //! Adds a target
  inline void addTarget(const QString &target) { mTargets.push_back(target); }

private: 
  //! Loads a target configuration
  bool loadTarget(const GameConfigNode &root, const QString &target);

  //! The project
  GameProject *mProject;

  //! List of all targets to include
  QStringList mTargets;
};

}