#include <maker/game/GameProject.hpp>
#include <maker/project/MakerProject.hpp>
#include <maker/project/ProjectLocalConfiguration.hpp>
#include <aoba/yaml/YamlTools.hpp>
#include <yaml-cpp/yaml.h>

// -----------------------------------------------------------------------------
bool ProjectLocalConfiguration::load() {
  if (!mProject) {
    return false;
  }

  YAML::Node config;
  try {
    config = YAML::LoadFile(fileName().toStdString().c_str());
  } catch(YAML::BadFile&) {
    return false;
  }

  mZoom = Aoba::Yaml::asDouble(config["zoom"], 2.0);
  mActiveAssetId = Aoba::Yaml::asString(config["active_asset"], "");
  mLoadedAssetIds.clear();
  if (Aoba::Yaml::isArray(config["loaded_assets"])) {
    for (const auto &node : config["loaded_assets"]) {
      mLoadedAssetIds.push_back(Aoba::Yaml::asString(node));
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
void ProjectLocalConfiguration::save() {
  if (!mProject) {
    return;
  }

  YAML::Emitter root;
  root << YAML::BeginMap;
  root << YAML::Key << "zoom" << YAML::Value << mZoom.get();
  root << YAML::Key << "loaded_assets" << YAML::Value << YAML::BeginSeq;
  for (const auto &asset : mLoadedAssetIds) {
    root << asset.toStdString();
  }
  root << YAML::EndSeq;
  if (!mActiveAssetId.isEmpty()) {
    root << YAML::Key << "active_asset" << YAML::Value << mActiveAssetId.toStdString();
  }
  root << YAML::EndMap;

  QFile file(fileName());
  if (!file.open(QIODevice::WriteOnly)) {
    return;
  }

  file.write(root.c_str(), root.size());
}

// -----------------------------------------------------------------------------
QString ProjectLocalConfiguration::fileName() {
  return mProject->gameProject()->path().filePath(".localconfig.yml");
}

// -----------------------------------------------------------------------------
