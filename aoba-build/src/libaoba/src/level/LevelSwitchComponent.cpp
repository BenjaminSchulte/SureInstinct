#include <aoba/log/Log.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/output/DynamicBuffer.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <fma/symbol/CalculatedNumber.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/tilemap/TilemapConfig.hpp>
#include <aoba/tilemap/Tilemap.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/level/Level.hpp>
#include <aoba/level/LevelConfig.hpp>
#include <aoba/level/LevelSwitchComponent.hpp>
#include <aoba/level/LevelTilemapComponent.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool LevelSwitchComponent::resize(Level *level, int left, int top, int, int, const QSize &newSize) const {
  const LevelSwitchInstance &original = level->properties()[mId].value<LevelSwitchInstance>();
  LevelSwitchInstance copy;

  QMapIterator<QString, QVector<LevelSwitch>> it(original.mSwitches);
  while (it.hasNext()) {
    it.next();

    QVector<LevelSwitch> newList;
    for (LevelSwitch sw : it.value()) {
      sw.x += left;
      sw.y += top;

      if (sw.x < newSize.width() && sw.y < newSize.height()) {
        newList.push_back(sw);
      }
    }

    copy.mSwitches.insert(it.key(), newList);
  }
  
  QVariant value;
  value.setValue(copy);
  level->properties().insert(mId, value);
  return true;
}

// -----------------------------------------------------------------------------
bool LevelSwitchComponent::save(Level *level, YAML::Emitter &root) const {
  const LevelSwitchInstance &instance = level->properties()[mId].value<LevelSwitchInstance>();
  if (instance.mSwitches.isEmpty()) {
    return true;
  }

  LevelTilemapComponent *tilemapComponent = level->assetSet()->getComponent<LevelTilemapComponent>();
  Tilemap *tilemap = tilemapComponent->tilemap(level);
  if (!tilemap) {
    Aoba::log::fatal("Switches component must be load after tilemap component");
    return false;
  }

  root << YAML::Key << "switches" << YAML::Value << YAML::BeginMap;
  QMapIterator<QString, QVector<LevelSwitch>> it(instance.mSwitches);
  while (it.hasNext()) {
    it.next();

    root << YAML::Key << it.key().toStdString() << YAML::Value << YAML::Flow << YAML::BeginSeq;
    for (const auto &tile : it.value()) {
      root << YAML::Key << "x" << YAML::Value << tile.x;
      root << YAML::Key << "y" << YAML::Value << tile.y;
      if (tile.flipX) { root << YAML::Key << "flipX" << YAML::Value << true; }
      if (tile.flipY) { root << YAML::Key << "flipY" << YAML::Value << true; }

      // !TILE IS MISSING!
      // !LAYER IS MISSING
    }
    root << YAML::EndSeq;
  }
  root << YAML::EndMap;

  Aoba::log::debug("Switches won't work yet (SAVE)");
  return false;
}

// -----------------------------------------------------------------------------
bool LevelSwitchComponent::load(Level *level, YAML::Node &config) const {
  LevelTilemapComponent *tilemapComponent = level->assetSet()->getComponent<LevelTilemapComponent>();
  Tilemap *tilemap = tilemapComponent->tilemap(level);
  if (!tilemap) {
    Aoba::log::warn("Switches component must be load after tilemap component");
    return false;
  }

  LevelSwitchInstance instance;
  for (const auto &pair : config["switches"]) {
    QString id = Yaml::asString(pair.first);

    QVector<LevelSwitch> list;
    for (const auto &item : pair.second) {
      QString layerId = Yaml::asString(item["layer"]);
      auto *layer = tilemap->assetSet()->layer(layerId);
      if (!layer) {
        Aoba::log::error("Unable to find layer in tilemap: " + layerId);
        return false;
      }

      TilesetTileRef tile = tilemap->tileset()->layer(layer->tilesetLayer())->require(Yaml::asInt(item["tile"]), true);

      list.push_back(LevelSwitch(
        Yaml::asInt(item["x"]),
        Yaml::asInt(item["y"]),
        tile.index,
        tile.flipX ^ Yaml::asBool(item["flipX"]),
        tile.flipY ^ Yaml::asBool(item["flipY"]),
        layer->index()
      ));
    }
    instance.mSwitches.insert(id, list);
  }

  QVariant value;
  value.setValue(instance);
  level->properties().insert(mId, value);
  return true;
}

// -----------------------------------------------------------------------------
QString LevelSwitchComponent::headerSymbolName(Level *level) const {
  return level->symbolName() + "__" + mId;
}

// -----------------------------------------------------------------------------
QString LevelSwitchComponent::symbolName(Level *level, const QString &id) const {
  return headerSymbolName(level) + "__" + id;
}

// -----------------------------------------------------------------------------
bool LevelSwitchComponent::build(Level *level, FMA::linker::LinkerBlock *block) const {
  const LevelSwitchInstance &instance = level->properties()[mId].value<LevelSwitchInstance>();
  if (instance.mSwitches.isEmpty()) {
    block->writeNumber(0, 1);
    return true;
  }
  
  FMA::linker::LinkerBlock *changes = level->assetSet()->assetLinkerObject().createLinkerBlock(headerSymbolName(level));
  FMA::symbol::ReferencePtr symbolLevel(new FMA::symbol::SymbolReference(headerSymbolName(level).toStdString()));
  FMA::symbol::ReferencePtr symbol16(new FMA::symbol::ConstantNumber(16));
  FMA::symbol::ReferencePtr symbolBank(new FMA::symbol::CalculatedNumber(symbolLevel, FMA::symbol::CalculatedNumber::RSHIFT, symbol16));
  block->write(symbolBank, 1);

  QMapIterator<QString, QVector<LevelSwitch>> it(instance.mSwitches);
  while (it.hasNext()) {
    it.next();

    auto list = it.value();

    std::sort(list.begin(), list.end(), [](const LevelSwitch &left, const LevelSwitch &right) -> bool {
      if (left.y < right.y) { return true; }
      if (left.x < right.x) { return true; }
      return false;
    });

    int lastX = 1000000;
    int lastY = 1000000;
    int lastTile = 1000000;

    // Position:
    //   0xxxxxxx
    //   ryyyyyyy (r=tile is relative)
    // Or
    //   1rxxxyyy (r=tile is relative, x is signed, y is unsigned)

    // Tile:
    //   XYtttttt tttttttt
    // Or
    //   XYtttttt (relative, signed)

    changes->symbol(symbolName(level, it.key()).toStdString());
    changes->writeNumber(list.count(), 1);

    QVector<bool> layerList;
    for (const auto &item : list) {
      layerList << item.layer;
    }    
    
    int8_t layerLeft = 0;
    uint8_t layerIndex = 0;

    for (const auto &item : list) {
      if (layerLeft-- <= 0) {
        uint8_t bitmask = 0;
        uint8_t bitmaskBit = 1;
        
        while (layerIndex < layerList.count() && layerLeft < 8) {
          bitmask |= layerList[layerIndex++] ? bitmaskBit : 0;
          layerLeft++;
          bitmaskBit <<= 1;
        }

        changes->writeNumber(bitmask, 1);
      }

      int relX = item.x - lastX;
      int relY = item.y - lastY;
      int relTile = item.tile - lastTile;

      bool isRelX = relX >= -4 && relX <= 3;
      bool isRelY = relY >= 0 && relY <= 7;
      bool isRelTile = relTile >= -32 && relTile <= 31;

      uint8_t flip = (item.flipX ? 0x80 : 0) | (item.flipY ? 0x40 : 0);

      if (isRelX && isRelY) {
        changes->writeNumber(0x80 | (isRelTile ? 0x40 : 0) | (relX & 0x7) << 3 | (relY & 0x7), 1);
      } else {
        changes->writeNumber(item.x, 1);
        changes->writeNumber(item.y | (isRelTile ? 0x80 : 0), 1);
      }

      if (isRelTile) {
        changes->writeNumber(flip | (relTile & 0x3F), 1);
      } else {
        changes->writeNumber((flip << 8) | item.tile, 2);
      }

      lastX = item.x;
      lastY = item.y;
      lastTile = item.tile;
    }
  }

  return true;
}
