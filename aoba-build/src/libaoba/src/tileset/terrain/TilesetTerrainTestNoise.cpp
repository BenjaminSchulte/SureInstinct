#include <aoba/log/Log.hpp>
#include <QPoint>
#include <aoba/tilemap/TilemapTerrain.hpp>
#include <aoba/tileset/TilesetTerrainTestNoise.hpp>
#include <aoba/yaml/YamlTools.hpp>
#include <aoba/utils/AobaRandom.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool TilesetTerrainTestNoise::test(TilemapTerrainLayer*, TilemapTerrainBuildContext &, const QPoint &pos) const {
  return AobaRandom(mSeed + pos.x() + pos.y() * 5000).random() < mPercentage;
}

// -----------------------------------------------------------------------------
bool TilesetTerrainTestNoise::load(const YAML::Node &config) {
  mSeed = Yaml::asInt(config["seed"], 13371337);
  mPercentage = Yaml::asDouble(config["percent"], 0.5);
  return true;
}

// -----------------------------------------------------------------------------
bool TilesetTerrainTestNoise::save(YAML::Emitter &) const {
  Aoba::log::debug("TODO: SAVE TilesetTerrainTestNoise");
  return false;
}
