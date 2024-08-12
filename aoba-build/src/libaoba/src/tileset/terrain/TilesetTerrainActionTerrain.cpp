#include <aoba/log/Log.hpp>
#include <aoba/tilemap/TilemapTerrain.hpp>
#include <aoba/tilemap/TilemapTerrainLayer.hpp>
#include <aoba/tileset/TilesetTerrainActionTerrain.hpp>
#include <aoba/tileset/TilesetTerrainTestTag.hpp>
#include <aoba/tileset/TilesetTerrain.hpp>
#include <aoba/tileset/TilesetTerrainLayer.hpp>
#include <aoba/tileset/TilesetTerrainRule.hpp>
#include <aoba/tileset/TilesetTerrainRuleGroup.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
void TilesetTerrainActionTerrain::apply(TilemapTerrainLayer *layer, TilemapTerrainBuildContext &context, const QPoint &pos) const {
  QString tilesetLayer = context.terrainLayers[layer].terrainLayer->defaultLayer()->config()->id();
  if (!context.layers.contains(tilesetLayer)) {
    return;
  }

  TilesetTerrainTestTag test(mRule);
  test.setTags(mTestTags);

  int shift = 0;
  int value = 0;
  for (int y=-1; y<=1; y++) {
    for (int x=-1; x<=1; x++) {
      if (x == 0 && y == 0) {
        continue;
      }

      QPoint newPos(pos.x() + x, pos.y() + y);
      if (newPos.x() >= 0 && newPos.y() >= 0 && newPos.x() < layer->width() && newPos.y() < layer->height()) {
        if (test.test(layer, context, newPos)) {
          value |= 1 << shift;
        }
      } else {
        value |= 1 << shift;
      }

      shift++;
    }
  }

  for (const auto &group : mGroups) {
    if ((value & group.mask) != group.compare) {
      continue;
    }

    group.stamp.apply(layer, context, pos);
    if (mLast) {
      int targetIndex = pos.x() + pos.y() * layer->width();
      context.terrainLayers[layer].stopped[targetIndex] = true;
    }
    break;
  }
}

// -----------------------------------------------------------------------------
bool TilesetTerrainActionTerrain::load(const YAML::Node &config) {
  Tileset *tileset = mRule->group()->terrain()->tileset();

  if (!Yaml::isArray(config["tiles"])) {
    Aoba::log::warn("Missing tiles for terrain");
    return false;
  }

  mSeed = Yaml::asInt(config["seed"], 123312312);
  mLast = Yaml::asBool(config["last"]);

  for (const auto &nextTo : config["next_to"]) {
    mTestTags.push_back(mRule->group()->terrain()->getTag(Yaml::asString(nextTo)));
  }

  for (const auto &group : config["tiles"]) {
    for (const auto &match : group) {
      QString key(match.first.as<std::string>().c_str());
      if (key.size() != 15 || key.left(5) != "tile_" || key[8] != '_' || key[11] != '_') {
        Aoba::log::warn("Terrain tile groups must have format tile_xxx_xx_xxx but is " + key);
        continue;
      }

      TilesetTerrainActionTerrainGroup newGroup;
      QString bitmask = key.mid(5, 3) + key.mid(9, 2) + key.mid(12, 3);
      for (int i=0; i<8; i++) {
        if (bitmask[i] == 'x') {
        } else if (bitmask[i] == '0') {
          newGroup.mask |= 1 << i;
        } else if (bitmask[i] == '1') {
          newGroup.mask |= 1 << i;
          newGroup.compare |= 1 << i;
        } else {
          Aoba::log::warn("Terrain tile groups must have format tile_xxx_xx_xxx but is " + key);
          continue;
        }

        if (!newGroup.stamp.load(tileset, match.second)) {
          return false;
        }
      }

      mGroups.push_back(newGroup);
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TilesetTerrainActionTerrain::save(YAML::Emitter &) const {
  Aoba::log::debug("TODO: SAVE TilesetTerrainActionTerrain");
  return false;
}
