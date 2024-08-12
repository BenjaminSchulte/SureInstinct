#include <aoba/log/Log.hpp>
#include <aoba/tilemap/TilemapTerrain.hpp>
#include <aoba/tilemap/TilemapTerrainLayer.hpp>
#include <aoba/tileset/TilesetTerrain.hpp>
#include <aoba/tileset/TilesetTerrainRule.hpp>
#include <aoba/tileset/TilesetTerrainRuleGroup.hpp>
#include <aoba/tileset/TilesetTerrainTestNeighbourCount.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool TilesetTerrainTestNeighbourCount::test(TilemapTerrainLayer *layer, TilemapTerrainBuildContext &context, const QPoint &pos) const {
  bool left = TilesetTerrainTestTag::test(layer, context, QPoint(pos.x() - mDistance, pos.y()));
  bool right = TilesetTerrainTestTag::test(layer, context, QPoint(pos.x() + mDistance, pos.y()));
  bool top = TilesetTerrainTestTag::test(layer, context, QPoint(pos.x(), pos.y() - mDistance));
  bool bottom = TilesetTerrainTestTag::test(layer, context, QPoint(pos.x(), pos.y() + mDistance));

  int count = 0;
  if (left) { count++; }
  if (top) { count++; }
  if (right) { count++; }
  if (bottom) { count++; }
  
  return count >= mMinimum && count <= mMaximum;
}

// -----------------------------------------------------------------------------
bool TilesetTerrainTestNeighbourCount::load(const YAML::Node &config) {
  mMinimum = Yaml::asInt(config["minimum"], 0);
  mMaximum = Yaml::asInt(config["maximum"], 4);
  mDistance = Yaml::asInt(config["distance"], 0);

  return TilesetTerrainTestTag::load(config);
}

// -----------------------------------------------------------------------------
bool TilesetTerrainTestNeighbourCount::save(YAML::Emitter &config) const {
  Aoba::log::debug("TODO: SAVE TilesetTerrainTestNeighbourCount");
  return TilesetTerrainTestTag::save(config);
}
