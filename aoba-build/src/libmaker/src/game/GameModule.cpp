#include <aoba/log/Log.hpp>
#include <maker/game/GameModule.hpp>
#include <maker/game/GameModuleList.hpp>
#include <maker/game/GameProject.hpp>

using namespace Maker;

// -----------------------------------------------------------------------------
bool GameModule::load() {
  for (const GameModuleDependency &dependency : mHeader.dependencies()) {
    if (!loadDependency(dependency)) {
      return false;
    }
  }

  if (QFileInfo(mHeader.path().absoluteFilePath("properties.yml")).exists()) {
    if (!mProperties.load(mHeader.path().absoluteFilePath("properties.yml"), mHeader.id() + ".")) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
QString GameModule::buildFmaCode(GameProject *project) const {
  QStringList list;

  QMapIterator<QString, GameModulePropertyDefinition> it(mProperties.properties());
  while (it.hasNext()) {
    it.next();

    if (!project->properties().contains(it.key())) {
      continue;
    }

    list << it.value().buildFmaCode(project->properties()[it.key()]);
  }

  for (const QString &include : mHeader.includes()) {
    list << ("require \"" + include + "\"");
  }

  return list.join('\n');
}

// -----------------------------------------------------------------------------
bool GameModule::loadDependency(const GameModuleDependency &dependency) {
  GameModule *mod = mModuleList->load(dependency);
  if (!mod) {
    return false;
  }

  for (GameModule *dep : mod->dependencies()) {
    if (mLoadedDependendModules.contains(dep->header().id())) {
      continue;
    }

    mDependendModules.push_back(dep);
    mLoadedDependendModules.push_back(dep->header().id());
  }

  if (!mLoadedDependendModules.contains(mod->header().id())) {
    mDependendModules.push_back(mod);
    mLoadedDependendModules.push_back(mod->header().id());
  }

  return true;
}