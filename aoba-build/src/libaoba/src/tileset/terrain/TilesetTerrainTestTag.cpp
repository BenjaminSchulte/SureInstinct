#include <aoba/log/Log.hpp>
#include <aoba/tilemap/TilemapTerrain.hpp>
#include <aoba/tilemap/TilemapTerrainLayer.hpp>
#include <aoba/tileset/TilesetTerrain.hpp>
#include <aoba/tileset/TilesetTerrainRule.hpp>
#include <aoba/tileset/TilesetTerrainRuleGroup.hpp>
#include <aoba/tileset/TilesetTerrainTestTag.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool TilesetTerrainTestTag::test(TilemapTerrainLayer *layer, TilemapTerrainBuildContext &context, const QPoint &pos) const {
  if (pos.x() < 0 || pos.y() < 0 || pos.x() >= layer->width() || pos.y() >= layer->height()) {
    return false;
  }
  
  for (int tag : mTags) {
    if (!context.terrainLayers[layer].tags.test(pos, tag)) {
      return false;
    }
  }
  for (int tag : mNotTags) {
    if (context.terrainLayers[layer].tags.test(pos, tag)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TilesetTerrainTestTag::load(const YAML::Node &config) {
  mTags.clear();

  if (Yaml::isArray(config["is"])) {
    for (const auto &node : config["is"]) {
      mTags.push_back(mRule->group()->terrain()->getTag(Yaml::asString(node)));
    }
  }
  if (Yaml::isArray(config["is_not"])) {
    for (const auto &node : config["is_not"]) {
      mNotTags.push_back(mRule->group()->terrain()->getTag(Yaml::asString(node)));
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TilesetTerrainTestTag::save(YAML::Emitter &) const {
  Aoba::log::debug("TODO: SAVE TilesetTerrainTestTag");
  return false;
}
