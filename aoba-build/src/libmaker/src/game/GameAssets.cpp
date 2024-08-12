#include <QDebug>
#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameAssets.hpp>
#include <maker/game/GameAssetType.hpp>
#include <maker/game/GameProject.hpp>
#include <yaml-cpp/yaml.h>

using namespace Maker;
using namespace Aoba;

// -----------------------------------------------------------------------------
GameAssets::~GameAssets() {
  for (const GameAssetsMap &map : mAssets) {
    for (AbstractGameAsset *asset : map) {
      delete asset;
    }
  }
}

// -----------------------------------------------------------------------------
void GameAssets::add(AbstractGameAsset *asset) {
  mAssets[asset->abstractType()->id()].insert(asset->id(), asset);
}

// -----------------------------------------------------------------------------
bool GameAssets::load() {
  if (!loadGlobalAssets()) {
    return false;
  }

  GameConfigReader config(GameConfigReader::fromFile(mProject, mProject->configFile("assets.yml")));

  for (const auto *type : mProject->assetTypes().assetTypesInOrder()) {
    for (const QString &id : config[type->id()].keys()) {
      GameAsset<AbstractAsset> *asset = static_cast<GameAsset<AbstractAsset> *>(type->create(id));

      if (!asset->create(config[type->id()][id])) {
        Aoba::log::warn("Could not load " + type->id() + " " + id);
        delete asset;
        return false;
      }

      mAssets[type->id()].insert(id, asset);
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool GameAssets::loadGlobalAssets() {
  if (!QFileInfo(mProject->configFile("global_assets.yml")).exists()) {
    return true;
  }

  GameConfigReader config(GameConfigReader::fromFile(mProject, mProject->configFile("global_assets.yml")));
  for (const auto &item : config["exports"].asList()) {
    mGlobalAssets.push_back(GameGlobalAsset(
      item["as"].asString(),
      item["type"].asString(),
      item["id"].asString(),
      item["attribute"].asString()
    ));
  }

  return true;
}

// -----------------------------------------------------------------------------
bool GameAssets::save() {
  YAML::Emitter root;
  root << YAML::BeginMap;
  for (const auto *type : mProject->assetTypes().assetTypesInOrder()) {
    if (type->assetSet()->all().isEmpty()) {
      continue;
    }
    root << YAML::Comment("-----------------------------------------");

    root << YAML::Key << type->id().toStdString() << YAML::Value << YAML::BeginMap;
    for (auto *asset : mAssets[type->id()]) {
      root << YAML::Key << asset->id().toStdString() << YAML::Value;
      if (!getAsset(type->id(), asset->id())->toYaml(root)) {
        return false;
      }
    }
    root << YAML::EndMap << YAML::Newline << YAML::Newline;
  }
  root << YAML::EndMap;

  QFile file(mProject->configFile("assets.yml"));
  if (!file.open(QIODevice::WriteOnly)) {
    Aoba::log::warn("Unable to open assets.yml for writing");
    return false;
  }

  file.write(root.c_str(), root.size());
  
  return true;
}

// -----------------------------------------------------------------------------
bool GameAssets::loadAssets() {
  for (const auto *type : mProject->assetTypes().assetTypesInOrder()) {
    for (auto *asset : type->assetSet()->all()) {
      auto task(mProject->log().assetTask("LOAD", type->id(), asset->id()));

      if (!type->load(asset)) {
        return task.failed("Failed to load asset");
      }

      task.succeeded();
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool GameAssets::buildAssets() {
  for (const auto *type : mProject->assetTypes().assetTypesInOrder()) {
    for (auto *asset : type->assetSet()->all()) {
      auto task(mProject->log().assetTask("BUILD", type->id(), asset->id()));

      if (!type->build(asset)) {
        return task.failed("Failed to build asset");
      }

      task.succeeded();
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool GameAssets::prepareGlobalAssets() {
  for (const auto &item : mGlobalAssets) {
    GameAssetType<Aoba::AbstractAsset> *set = mProject->assetTypes().getAbstract(item.type());
    if (!set) {
      Aoba::log::error("Unknown asset type: " + item.type());
      return false;
    }

    if (!set->assetResolver()) {
      Aoba::log::error("Asset type has no attribute resolver: " + item.type());
      return false;
    }

    if (!set->assetResolver()->prepare(item.id(), item.attribute())) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool GameAssets::linkGlobalAssets(GameLinkResult &result) {
  for (const auto &item : mGlobalAssets) {
    GameAssetType<Aoba::AbstractAsset> *set = mProject->assetTypes().getAbstract(item.type());
    if (!set) {
      Aoba::log::error("Unknown asset type: " + item.type());
      return false;
    }

    if (!set->assetResolver()) {
      Aoba::log::error("Asset type has no attribute resolver: " + item.type());
      return false;
    }

    QVariant value = set->assetResolver()->resolve(item.id(), item.attribute());
    if (!value.isValid() || value.isNull()) {
      Aoba::log::error("Could not export asset attribute: " + item.type() + " " + item.id() + "/" + item.attribute());
      return false;
    }

    QStringList globalName = item.exportName().split("::");
    QString name = globalName.back();
    globalName.pop_back();
    
    QStringList itemResult;

    if (!globalName.isEmpty()) {
      itemResult << "module " + globalName.join("::");
    }

    if (value.type() == QVariant::String) {
      itemResult << "import " + value.toString();
      itemResult << name + "=" + value.toString();
    } else if (value.type() == QVariant::Int) {
      itemResult << name + "=" + QString::number(value.toInt());
    } else {
      Aoba::log::error("Unsupport result type for attribute");
      return false;
    }

    if (!globalName.isEmpty()) {
      itemResult << "end";
    }

    result.addCode(itemResult.join('\n'));
  }

  return true;
}

// -----------------------------------------------------------------------------
bool GameAssets::linkAssets(GameLinkResult &result) {
  if (!linkGlobalAssets(result)) {
    return false;
  }

  for (const auto *type : mProject->assetTypes().assetTypesInOrder()) {
    for (auto *asset : type->assetSet()->all()) {
      auto task(mProject->log().assetTask("LINK", type->id(), asset->id()));

      if (!type->link(asset, result)) {
        return task.failed("Failed to link asset");
      }

      task.succeeded();
    }
  }

  return true;
} 

// -----------------------------------------------------------------------------
