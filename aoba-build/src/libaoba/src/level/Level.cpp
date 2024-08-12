#include <aoba/level/Level.hpp>
#include <aoba/level/LevelComponent.hpp>
#include <aoba/yaml/YamlTools.hpp>
#include <aoba/QtCompatibility.hpp>
#include <aoba/utils/AobaPath.hpp>
#include <yaml-cpp/yaml.h>

using namespace Aoba;

// -----------------------------------------------------------------------------
Level::Level(LevelAssetSet *set, const QString &id, const QDir &dir)
  : Asset(set, id)
  , mPath(dir)
{
  set->add(this);
}

// -----------------------------------------------------------------------------
Level::~Level() {
}

// -----------------------------------------------------------------------------
QString Level::symbolName() const {
  return "__asset_level_" + mId;
}

// -----------------------------------------------------------------------------
QString Level::scriptNamespace() const {
  QStringList parts = mId.split('_', QT_SKIP_EMPTY_PARTS);
  for (int i=0; i<parts.size(); i++) {
    parts[i].replace(0, 1, parts[i][0].toUpper());
  }

  return "GameLevel::" + parts.join("");
}

// -----------------------------------------------------------------------------
QString Level::levelScriptName(const QString &scriptName) const {
  if (scriptName.isEmpty()) {
    return "";
  }

  if (scriptName[0] == '.') {
    return scriptNamespace() + "::" + scriptName.mid(1);
  }

  return scriptName;
}

// -----------------------------------------------------------------------------
bool Level::save() {
  if (!createWritablePath(mPath)) {
    Aoba::log::error("Unable to create path for level writing");
    return false;
  }

  YAML::Emitter root;
  root << YAML::BeginMap;

  root << YAML::Key << "name" << YAML::Value << mName.toStdString();

  root << YAML::Key << "scripts" << YAML::BeginSeq;
  for (const QString &script : mScripts) {
    root << mPath.relativeFilePath(script).toStdString();
  }
  root << YAML::EndSeq;

  for (LevelComponent *component : mAssetSet->components()) {
    if (!component->save(this, root)) {
      Aoba::log::error("Failed to save level component " + component->typeId());
      return false;
    }
  }

  root << YAML::EndMap;
  QFile file(mPath.filePath("level.yml"));
  if (!file.open(QIODevice::WriteOnly)) {
    Aoba::log::error("Failed to open level.yml for writing");
    return false;
  }

  file.write(root.c_str(), root.size());

  return true;
}

// -----------------------------------------------------------------------------
bool Level::reload() {
  YAML::Node config;
  try {
    config = YAML::LoadFile(mPath.filePath("level.yml").toStdString().c_str());
  } catch(YAML::BadFile&) {
    return false;
  }

  mName = Yaml::asString(config["name"], mId);
  for (const YAML::Node &node : config["scripts"]) {
    mScripts.push_back(mPath.filePath(Yaml::asString(node)));
  }

  for (LevelComponent *component : mAssetSet->components()) {
    if (!component->load(this, config)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool Level::resize(int left, int top, int right, int bottom, const QSize &newSize) {
  for (LevelComponent *component : mAssetSet->components()) {
    if (!component->resize(this, left, top, right, bottom, newSize)) {
      Aoba::log::error("Failed to resize level component " + component->typeId());
      return false;
    }
  }
  return true;
}

// -----------------------------------------------------------------------------
bool Level::build() {
  FMA::linker::LinkerBlock *block = mAssetSet->assetLinkerObject().createLinkerBlock(symbolName(), mAssetSet->headerRomBank());

  for (LevelComponent *component : mAssetSet->components()) {
    if (!component->build(this, block)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
QStringList Level::getFmaObjectFiles() const {
  return mAssetSet->assetLinkerObject().getFmaObjectFiles();
}

// -----------------------------------------------------------------------------
QString Level::getFmaCode() const {
  /*QStringList result;

  for (LevelComponent *component : mAssetSet->components()) {
    QString row(component->getFmaCode(this));
    
    if (!row.isEmpty()) {
      result << row;
    }
  }

  QString properties(propertiesToFma(mProperties));
  result << mAssetSet->assetName() + ".load_compiled_binary (" + properties + ")";

  return result.join('\n');*/
  return "";
}

// -----------------------------------------------------------------------------
QString Level::propertiesToFma(const QMap<QString, QVariant> &map) const {
  QMapIterator<QString, QVariant> it(map);

  QStringList members;

  while (it.hasNext()) {
    it.next();

    const QVariant &v(it.value());
    if (v.canConvert<int>()) {
      members << it.key() + ": " + QString::number(v.value<int>());
    } else {
      members << it.key() + ": nil";
    }
  }

  return "{" + members.join(',') + "}";
}