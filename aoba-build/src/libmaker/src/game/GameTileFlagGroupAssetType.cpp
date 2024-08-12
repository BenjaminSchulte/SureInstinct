#include <aoba/log/Log.hpp>
#include <aoba/tileset/TileFlagGroupEnumParameter.hpp>
#include <aoba/tileset/TileFlagGroupBooleanParameter.hpp>
#include <aoba/tileset/TileFlagGroupDefaultGenerator.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GamePaletteAssetType.hpp>
#include <maker/game/GamePaletteGroupAssetType.hpp>
#include <maker/game/GameTileFlagGroupAssetType.hpp>

using namespace Maker;
using namespace Aoba;

// -----------------------------------------------------------------------------
Aoba::TileFlagGroup *GameTileFlagGroupAsset::configure(const GameConfigNode &config) {
  (void)config;
  return nullptr;
}


// -----------------------------------------------------------------------------
GameTileFlagGroupAssetType::~GameTileFlagGroupAssetType() {
}

// -----------------------------------------------------------------------------
TileFlagGroupAssetSet *GameTileFlagGroupAssetType::configure(const GameConfigNode &config) {
  TileFlagGroupAssetSet *set = new TileFlagGroupAssetSet(mProject->project(), mId, config["data_size"].asInt(8));

  if (!addGenerators(set, config["generate"])) {
    delete set;
    return nullptr;
  }

  if (!addParameters(set, config["parameter"])) {
    delete set;
    return nullptr;
  }

  return set;
}

// -----------------------------------------------------------------------------
bool GameTileFlagGroupAssetType::addGenerators(Aoba::TileFlagGroupAssetSet *set, const GameConfigNode &list) const {
  for (const auto &item : list.asList()) {
    if (!addDefaultGenerator(set, item)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool GameTileFlagGroupAssetType::addDefaultGenerator(Aoba::TileFlagGroupAssetSet *set, const GameConfigNode &node) const {
  TileFlagGroupDefaultGeneratorConfig config;
  config.numericParameter = node["numeric_parameter"].asString();

  set->addManagedGenerator(new TileFlagGroupDefaultGenerator(config));

  return true;
}


// -----------------------------------------------------------------------------
bool GameTileFlagGroupAssetType::addParameters(Aoba::TileFlagGroupAssetSet *set, const GameConfigNode &list) const {
  for (const QString &id : list.keys()) {
    const auto &item = list[id];
    QString type = item["type"].asString();
    if (type == "enum" && addEnumParameter(set, id, item)) {
    } else if (type == "boolean" && addBooleanParameter(set, id, item)) {
    } else {
      Aoba::log::error("Unable to add parameter of type " + type);
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool GameTileFlagGroupAssetType::addEnumParameter(Aoba::TileFlagGroupAssetSet *set, const QString &id, const GameConfigNode &config) const {
  TileFlagGroupEnumParameter *par = new TileFlagGroupEnumParameter(config["name"].asString(id));

  for (const QString &key : config["values"].keys()) {
    const auto &item = config["values"][key];

    par->addOption(key, item["id"].asInt(0), item["name"].asString(key));
  }

  par->setDefaultValue(config["defaults_to"].asString(""));
  set->addManagedParameter(id, par);
  return true;
}

// -----------------------------------------------------------------------------
bool GameTileFlagGroupAssetType::addBooleanParameter(Aoba::TileFlagGroupAssetSet *set, const QString &id, const GameConfigNode &config) const {
  TileFlagGroupBooleanParameter *par = new TileFlagGroupBooleanParameter(config["name"].asString(id));

  par->setDefaultValue(config["defaults_to"].asBool());
  par->setTrueValue(config["true_value"].asInt(1));
  par->setFalseValue(config["false_value"].asInt(0));
  set->addManagedParameter(id, par);
  return true;
}

// -----------------------------------------------------------------------------
GameTileFlagGroupAsset *GameTileFlagGroupAssetType::create(const QString &id) const {
  return new GameTileFlagGroupAsset(this, mProject, id);
}

// -----------------------------------------------------------------------------
