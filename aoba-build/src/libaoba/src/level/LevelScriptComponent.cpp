#include <aoba/log/Log.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/level/Level.hpp>
#include <aoba/level/LevelConfig.hpp>
#include <aoba/level/LevelScriptComponent.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool LevelScriptComponent::save(Level *level, YAML::Emitter &root) const {
  for (const QString &id : mScripts) {
    if (!level->properties().contains(mId + ":" + id)) {
      continue;
    }

    QString scriptName = level->properties()[mId + ":" + id].toString();
    root << YAML::Key << id.toStdString() << YAML::Value << scriptName.toStdString();
  }

  return true;
}

// -----------------------------------------------------------------------------
bool LevelScriptComponent::load(Level *level, YAML::Node &config) const {
  for (const QString &id : mScripts) {
    if (!config[id.toStdString()].IsDefined()) {
      continue;
    }

    QString scriptName = Yaml::asString(config[id.toStdString()]);
    level->properties()[mId + ":" + id] = scriptName;
  }

  return true;
}

// -----------------------------------------------------------------------------
bool LevelScriptComponent::build(Level *level, FMA::linker::LinkerBlock *block) const {
  for (const QString &id : mScripts) {
    const QVariant &v = level->properties()[mId + ":" + id];
    if (!v.isValid()) {
      block->writeNumber(0, 3);
      continue;
    }

    QString scriptName = level->assetSet()->project()->getScriptSymbolName(level->levelScriptName(v.value<QString>()));
    if (scriptName.isEmpty()) {
      return false;
    }

    block->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(scriptName.toStdString())), 3);
  }

  return true;
}
