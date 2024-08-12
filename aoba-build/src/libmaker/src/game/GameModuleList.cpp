#include <QDebug>
#include <aoba/log/Log.hpp>
#include <maker/game/GameModule.hpp>
#include <maker/game/GameModuleRegistry.hpp>
#include <maker/game/GameModuleHeader.hpp>
#include <maker/game/GameModuleList.hpp>

using namespace Maker;

// -----------------------------------------------------------------------------
GameModuleList::~GameModuleList() {
  deleteAll();
}

// -----------------------------------------------------------------------------
void GameModuleList::deleteAll() {
  qDebug() << "TODO: GameModuleList::deleteAll";
}

// -----------------------------------------------------------------------------
GameModule *GameModuleList::load(const QDir &path) {
  if (!mRegistry->loadModuleHeader(path)) {
    Aoba::log::error("Could not load project configuration");
    return nullptr;
  }

  const GameModuleHeader *header = mRegistry->moduleByPath(path);
  return load(*header);
}

// -----------------------------------------------------------------------------
GameModule *GameModuleList::load(const GameModuleDependency &dependency) {
  const GameModuleHeader *header = mRegistry->find(dependency);
  if (!header) {
    Aoba::log::error(QString("Could not find module %1 in version %2").arg(dependency.id()).arg(dependency.version().toString()));
    return nullptr;
  }

  return load(*header);
}

// -----------------------------------------------------------------------------
GameModule *GameModuleList::load(const GameModuleHeader &header) {
  QString hash = header.fullId();
  if (mModules.contains(hash)) {
    return mModules[hash];
  }

  GameModule *mod = new GameModule(this, header);
  Aoba::log::info(QString("Loading module: %1").arg(header.fullId()));
  if (!mod->load()) {
    delete mod;
    return nullptr;
  }

  mModules.insert(header.fullId(), mod);
  return mod;
}
