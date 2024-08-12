#include <aoba/log/Log.hpp>
#include <aoba/tileset/TilesetTerrain.hpp>
#include <aoba/tileset/TilesetTerrainType.hpp>
#include <aoba/tileset/TilesetTerrainLayer.hpp>
#include <aoba/tileset/TilesetTerrainRuleGroup.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
TilesetTerrain::TilesetTerrain(Tileset *tileset)
  : mTileset(tileset)
{
}

// -----------------------------------------------------------------------------
TilesetTerrain::~TilesetTerrain() {
  clear();
}

// -----------------------------------------------------------------------------
void TilesetTerrain::clear() {
  for (TilesetTerrainLayer *layer : mLayers) {
    delete layer;
  }
  for (TilesetTerrainType *type : mTypes) {
    delete type;
  }
  for (TilesetTerrainRuleGroup *group : mGroups) {
    delete group;
  }

  mGroups.clear();
  mLayers.clear();
  mTypes.clear();
  mTypeByIndex.clear();
}

// -----------------------------------------------------------------------------
TilesetTerrainLayer *TilesetTerrain::createLayer(const QString &id, const QString &name) {
  if (mLayers.contains(id)) {
    return mLayers[id];
  }

  TilesetTerrainLayer *type = new TilesetTerrainLayer(this, id, name);
  mLayers.insert(id, type);
  return type;
}

// -----------------------------------------------------------------------------
TilesetTerrainType *TilesetTerrain::createType(const QString &id, const QString &name, int icon, int index) {
  if (mTypes.contains(id)) {
    return mTypes[id];
  }

  TilesetTerrainType *type = new TilesetTerrainType(this, id, name, icon, index);
  mTypes.insert(id, type);
  mTypeByIndex.insert(type->index(), type);
  return type;
}

// -----------------------------------------------------------------------------
TilesetTerrainRuleGroup *TilesetTerrain::createGroup(const QString &name) {
  TilesetTerrainRuleGroup *group = new TilesetTerrainRuleGroup(this, name);
  mGroups.push_back(group);
  return group;
}

// -----------------------------------------------------------------------------
int TilesetTerrain::getTag(const QString &tag) {
  if (!mTags.contains(tag)) {
    mTags.insert(tag, mTags.size());
  }

  return mTags[tag];
}

// -----------------------------------------------------------------------------
bool TilesetTerrain::load(const YAML::Node &node) {
  clear();

  if (Yaml::isObject(node["layers"])) {
    for (const auto &typeConfig : node["layers"]) {
      QString id(typeConfig.first.as<std::string>().c_str());
      if (!createLayer(id, id)->load(typeConfig.second)) {
        return false;
      }
    }
  }

  if (Yaml::isObject(node["types"])) {
    for (const auto &typeConfig : node["types"]) {
      QString id(typeConfig.first.as<std::string>().c_str());
      if (!createType(id, id, 0, Yaml::asInt(typeConfig.second["id"]))->load(typeConfig.second)) {
        return false;
      }
    }
  }

  if (Yaml::isArray(node["rule_groups"])) {
    for (const auto &ruleConfig : node["rule_groups"]) {
      if (!createGroup(Yaml::asString(ruleConfig["name"]))->load(ruleConfig)) {
        return false;
      }
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TilesetTerrain::save(YAML::Emitter &) const {
  Aoba::log::debug("TODO: Save terrain");
  return false;
}
