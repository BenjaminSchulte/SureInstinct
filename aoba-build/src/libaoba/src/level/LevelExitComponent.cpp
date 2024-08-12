#include <QSize>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/level/Level.hpp>
#include <aoba/level/LevelConfig.hpp>
#include <aoba/level/LevelExitComponent.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool LevelExitComponent::resize(Level *level, int left, int top, int, int, const QSize &newSize) const {
  LevelExitInstance instance = level->properties()[mId].value<LevelExitInstance>();
  QVector<LevelExit> exits;
  for (auto &item : instance.mExits) {
    item.x += left;
    item.y += top;

    if (item.x < newSize.width() && item.y < newSize.height()) {
      exits.push_back(item);
    }
  }
  instance.mExits = exits;

  QVariant value;
  value.setValue(instance);
  level->properties().insert(mId, value);
  return true;
}

// -----------------------------------------------------------------------------
bool LevelExitComponent::save(Level *level, YAML::Emitter &root) const {
  const LevelExitInstance &instance = level->properties()[mId].value<LevelExitInstance>();
  if (instance.mExits.isEmpty()) {
    return true;
  }

  root << YAML::Key << "exits" << YAML::Value << YAML::BeginSeq;
  for (const auto &exit : instance.mExits) {
    root << YAML::BeginMap;
    root << YAML::Key << "at" << YAML::Value << YAML::Flow << YAML::BeginMap;
    root << YAML::Key << "x" << YAML::Value << exit.x;
    root << YAML::Key << "y" << YAML::Value << exit.y;
    root << YAML::EndMap;
    root << YAML::Key << "script" << YAML::Value << exit.script.toStdString();
    root << YAML::EndMap;
  }
  root << YAML::EndSeq;
  
  return true;
}

// -----------------------------------------------------------------------------
bool LevelExitComponent::load(Level *level, YAML::Node &config) const {
  LevelExitInstance instance;
  for (const auto &node : config["exits"]) {

    instance.mExits.push_back(LevelExit(
      Yaml::asInt(node["at"]["x"]),
      Yaml::asInt(node["at"]["y"]),
      Yaml::asString(node["script"])
    ));
  }

  QVariant value;
  value.setValue(instance);
  level->properties().insert(mId, value);
  return true;
}

// -----------------------------------------------------------------------------
bool LevelExitComponent::build(Level *level, FMA::linker::LinkerBlock *block) const {
  const LevelExitInstance &instance = level->properties()[mId].value<LevelExitInstance>();
  
  QString symbolName = level->symbolName() + "__" + mId;
  FMA::linker::LinkerBlock *exitList = level->assetSet()->assetLinkerObject().createLinkerBlock(symbolName, mBank);

  exitList->writeNumber(instance.mExits.size(), 1);
  for (const LevelExit &exit : instance.mExits) {
    uint16_t index = (exit.x << 1) | (exit.y << 7);
    exitList->writeNumber(index, 2);
  }

  for (const LevelExit &exit : instance.mExits) {
    QString exitName = level->assetSet()->project()->getScriptSymbolName(level->levelScriptName(exit.script));
    if (exitName.isEmpty()) {
      return false;
    }

    exitList->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(exitName.toStdString())), 3);
  }

  block->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(symbolName.toStdString())), 2);

  return true;
}
