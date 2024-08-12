#include <aoba/log/Log.hpp>
#include <aoba/tilemap/TilemapTerrain.hpp>
#include <aoba/tilemap/TilemapTerrainLayer.hpp>
#include <aoba/tileset/TilesetTerrain.hpp>
#include <aoba/tileset/TilesetTerrainRule.hpp>
#include <aoba/tileset/TilesetTerrainRuleGroup.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
TilesetTerrainRuleGroup::TilesetTerrainRuleGroup(TilesetTerrain *terrain, const QString &name)
  : mTerrain(terrain)
  , mName(name)
{
}

// -----------------------------------------------------------------------------
TilesetTerrainRuleGroup::~TilesetTerrainRuleGroup() {
  clear();
}

// -----------------------------------------------------------------------------
void TilesetTerrainRuleGroup::clear() {
  for (TilesetTerrainRule *rule : mRules) {
    delete rule;
  }
  mRules.clear();
}

// -----------------------------------------------------------------------------
TilesetTerrainRule *TilesetTerrainRuleGroup::createRule() {
  TilesetTerrainRule *rule = new TilesetTerrainRule(this);
  mRules.push_back(rule);
  return rule;
}

// -----------------------------------------------------------------------------
void TilesetTerrainRuleGroup::apply(TilemapTerrainLayer *layer, TilemapTerrainBuildContext &context, const QPoint &pos) const {
  int index = pos.x() + pos.y() * layer->width();
  TilemapTerrianBuildContextTerrainLayer &layerContext = context.terrainLayers[layer];

  for (const TilesetTerrainRule *rule : mRules) {
    if (layerContext.stopped[index]) {
      return;
    }

    rule->apply(layer, context, pos);
  }
}

// -----------------------------------------------------------------------------
bool TilesetTerrainRuleGroup::load(const YAML::Node &node) {
  mName = Yaml::asString(node["name"]);

  if (Yaml::isArray(node["rules"])) {
    for (const auto &ruleConfig : node["rules"]) {
      if (!createRule()->load(ruleConfig)) {
        return false;
      }
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TilesetTerrainRuleGroup::save(YAML::Emitter &) const {
  Aoba::log::debug("TODO: save TilesetTerrainRuleGroup");
  return false;
}
