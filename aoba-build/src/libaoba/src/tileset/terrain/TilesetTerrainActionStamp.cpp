#include <aoba/log/Log.hpp>
#include <aoba/tilemap/TilemapTerrain.hpp>
#include <aoba/tilemap/TilemapTerrainLayer.hpp>
#include <aoba/tileset/TilesetTerrainActionStamp.hpp>
#include <aoba/tileset/TilesetTerrain.hpp>
#include <aoba/tileset/TilesetTerrainLayer.hpp>
#include <aoba/tileset/TilesetTerrainRule.hpp>
#include <aoba/tileset/TilesetTerrainRuleGroup.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
void TilesetTerrainActionStamp::apply(TilemapTerrainLayer *layer, TilemapTerrainBuildContext &context, const QPoint &pos) const {
  mStamp.apply(layer, context, pos);

  if (mLast) {
    int targetIndex = pos.x() + pos.y() * layer->width();
    context.terrainLayers[layer].stopped[targetIndex] = true;
  }
}

// -----------------------------------------------------------------------------
bool TilesetTerrainActionStamp::load(const YAML::Node &config) {
  mLast = Yaml::asBool(config["last"]);
  return mStamp.load(mRule->group()->terrain()->tileset(), config);
}

// -----------------------------------------------------------------------------
bool TilesetTerrainActionStamp::save(YAML::Emitter &) const {
  Aoba::log::debug("TODO: SAVE TilesetTerrainActionStamp");
  return false;
}
