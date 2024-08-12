#include <QDebug>
#include <QDirIterator>
#include <QProcessEnvironment>
#include <aoba/log/Log.hpp>
#include <maker/game/GameModuleRegistry.hpp>
#include <maker/game/GameModuleHeader.hpp>

using namespace Maker;

// -----------------------------------------------------------------------------
GameModuleRegistry::GameModuleRegistry(GameModuleRegistry *parent)
  : mParent(parent)
{
  (void)mParent;
}

// -----------------------------------------------------------------------------
GameModuleRegistry::~GameModuleRegistry() {
  unloadAll();
}

// -----------------------------------------------------------------------------
void GameModuleRegistry::unloadAll() {
  QMapIterator<QString, QVector<GameModuleHeader*>> it(mModuleHeaders);
  while (it.hasNext()) {
    it.next();
    for (GameModuleHeader *header : it.value()) {
      delete header;
    }
  }

  mModuleHeaders.clear();
  mModuleByPath.clear();
}

// -----------------------------------------------------------------------------
void GameModuleRegistry::findModulesFromEnvironmentVariables() {
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  QString moduleList = env.value("AOBA_MODULE_PATH", "").trimmed();
  if (moduleList.isEmpty()) {
    Aoba::log::debug("Environment variable AOBA_MODULE_PATH is empty");
    return;
  }

  for (const QString &dir : moduleList.split(':')) {
    if (dir.isEmpty()) {
      continue;
    }

    findModules(dir);
  }
}

// -----------------------------------------------------------------------------
void GameModuleRegistry::findModules(const QDir &dir) {
  Aoba::log::trace(QString("Looking up modules in path: %1").arg(dir.absolutePath()));

  QDirIterator it(dir.absolutePath(), QDir::Dirs | QDir::NoDotAndDotDot);
  while (it.hasNext()) {
    QString path = it.next();

    if (QFileInfo(QDir(path).absoluteFilePath("module.yml")).exists()) {
      loadModuleHeader(path);
    }
  }
}

// -----------------------------------------------------------------------------
const GameModuleHeader *GameModuleRegistry::find(const GameModuleDependency &dep) const {
  for (const auto *header : mModuleHeaders[dep.id()]) {
    if (dep.version().matches(header->version())) {
      return header;
    }
  }

  if (mParent) {
    return mParent->find(dep);
  } else {
    return nullptr;
  }
}

// -----------------------------------------------------------------------------
bool GameModuleRegistry::loadModuleHeader(const QDir &dir) {
  if (hasModuleHeaderLoaded(dir)) {
    return true;
  }

  GameModuleHeader *header = new GameModuleHeader(dir);
  if (!header->load()) {
    delete header;
    return false;
  }

  mModuleHeaders[header->id()].push_back(header);
  mModuleByPath.insert(dir.absolutePath(), header);

  return true;
}