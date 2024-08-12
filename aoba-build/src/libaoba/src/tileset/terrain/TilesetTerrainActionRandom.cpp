#include <aoba/log/Log.hpp>
#include <QPoint>
#include <aoba/tileset/TilesetTerrainActionRandom.hpp>
#include <aoba/tileset/TilesetTerrainRule.hpp>
#include <aoba/yaml/YamlTools.hpp>
#include <aoba/utils/AobaRandom.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
TilesetTerrainActionRandom::~TilesetTerrainActionRandom() {
  clear();
}

// -----------------------------------------------------------------------------
void TilesetTerrainActionRandom::clear() {
  for (TilesetTerrainAction *action : mActions) {
    delete action;
  }
  mActions.clear();
}

// -----------------------------------------------------------------------------
void TilesetTerrainActionRandom::apply(TilemapTerrainLayer *layer, TilemapTerrainBuildContext &context, const QPoint &pos) const {
  if (mActions.isEmpty()) {
    return;
  }

  int index = AobaRandom(mSeed + pos.x() + pos.y() * 5000).random(mActions.size());
  mActions[index]->apply(layer, context, pos);
}

// -----------------------------------------------------------------------------
bool TilesetTerrainActionRandom::load(const YAML::Node &config) {
  mSeed = Yaml::asInt(config["seed"], 13371337);

  if (Yaml::isArray(config["select"])) {
    for (const auto &processNode : config["select"]) {
      QString typeId = Yaml::asString(processNode["type"], "stamp");
      TilesetTerrainAction *process = TilesetTerrainRule::createActionById(mRule, typeId);
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
bool TilesetTerrainActionRandom::save(YAML::Emitter &) const {
  Aoba::log::debug("TODO: SAVE TilesetTerrainActionRandom");
  return false;
}
