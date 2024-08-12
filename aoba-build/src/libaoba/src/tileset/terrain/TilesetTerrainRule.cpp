#include <aoba/log/Log.hpp>
#include <QPoint>
#include <aoba/tilemap/TilemapTerrain.hpp>
#include <aoba/tilemap/TilemapTerrainLayer.hpp>
#include <aoba/tileset/TilesetTerrainRule.hpp>
#include <aoba/tileset/TilesetTerrainTestTag.hpp>
#include <aoba/tileset/TilesetTerrainTestNoise.hpp>
#include <aoba/tileset/TilesetTerrainTestNeighbourCount.hpp>
#include <aoba/tileset/TilesetTerrainActionStamp.hpp>
#include <aoba/tileset/TilesetTerrainActionRandom.hpp>
#include <aoba/tileset/TilesetTerrainActionTerrain.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
TilesetTerrainRule::TilesetTerrainRule(TilesetTerrainRuleGroup *group)
  : mGroup(group)
{
}

// -----------------------------------------------------------------------------
TilesetTerrainRule::~TilesetTerrainRule() {
  clear();
}

// -----------------------------------------------------------------------------
void TilesetTerrainRule::clear() {
  for (TilesetTerrainTest *test : mConditions) {
    delete test;
  }
  for (TilesetTerrainAction *action : mActions) {
    delete action;
  }

  mConditions.clear();
  mActions.clear();
}

// -----------------------------------------------------------------------------
void TilesetTerrainRule::apply(TilemapTerrainLayer *layer, TilemapTerrainBuildContext &context, const QPoint &pos) const {
  for (TilesetTerrainTest *test : mConditions) {
    if (!test->test(layer, context, pos)) {
      return;
    }
  }

  int index = pos.x() + pos.y() * layer->width();
  for (TilesetTerrainAction *action : mActions) {
    if (context.terrainLayers[layer].stopped[index]) {
      break;
    }

    action->apply(layer, context, pos);
  }
}

// -----------------------------------------------------------------------------
TilesetTerrainAction *TilesetTerrainRule::createActionById(TilesetTerrainRule *parent, const QString &id) {
  if (id == TilesetTerrainActionStamp::TypeId()) {
    return new TilesetTerrainActionStamp(parent);
  } else if (id == TilesetTerrainActionRandom::TypeId()) {
    return new TilesetTerrainActionRandom(parent);
  } else if (id == TilesetTerrainActionTerrain::TypeId()) {
    return new TilesetTerrainActionTerrain(parent);
  } else {
    return nullptr;
  }
}

// -----------------------------------------------------------------------------
TilesetTerrainTest *TilesetTerrainRule::createConditionById(TilesetTerrainRule *parent, const QString &id) {
  if (id == TilesetTerrainTestTag::TypeId()) {
    return new TilesetTerrainTestTag(parent);
  } else if (id == TilesetTerrainTestNoise::TypeId()) {
    return new TilesetTerrainTestNoise(parent);
  } else if (id == TilesetTerrainTestNeighbourCount::TypeId()) {
    return new TilesetTerrainTestNeighbourCount(parent);
  } else {
    return nullptr;
  }
}

// -----------------------------------------------------------------------------
bool TilesetTerrainRule::load(const YAML::Node &config) {
  mCanFlipX = Yaml::asBool(config["flip_x"]);
  mCanFlipY = Yaml::asBool(config["flip_y"]);

  if (Yaml::isArray(config["test"])) {
    for (const auto &testNode : config["test"]) {
      QString typeId = Yaml::asString(testNode["type"], "tag");
      TilesetTerrainTest *test = createConditionById(this, typeId);
      if (!test) {
        Aoba::log::warn("Unable to create test condition of type " + typeId);
        return false;
      }

      mConditions.push_back(test);
      if (!test->load(testNode)) {
        return false;
      }
    }
  }

  if (Yaml::isArray(config["process"])) {
    for (const auto &processNode : config["process"]) {
      QString typeId = Yaml::asString(processNode["type"], "stamp");
      TilesetTerrainAction *process = createActionById(this, typeId);
      if (!process) {
        Aoba::log::warn("Unable to create process action of type " + typeId);
        return false;
      }

      mActions.push_back(process);
      if (!process->load(processNode)) {
        return false;
      }
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TilesetTerrainRule::save(YAML::Emitter &) const {
  Aoba::log::debug("TODO: save TilesetTerrainRule");
  return false;
}
