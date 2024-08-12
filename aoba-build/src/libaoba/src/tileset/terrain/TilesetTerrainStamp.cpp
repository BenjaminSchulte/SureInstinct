#include <QDebug>
#include <aoba/log/Log.hpp>
#include <aoba/tilemap/TilemapTerrain.hpp>
#include <aoba/tilemap/TilemapTerrainLayer.hpp>
#include <aoba/tileset/TilesetTerrainStamp.hpp>
#include <aoba/tileset/TilesetTerrain.hpp>
#include <aoba/tileset/TilesetTerrainLayer.hpp>
#include <aoba/tileset/TilesetTerrainRule.hpp>
#include <aoba/tileset/TilesetTerrainRuleGroup.hpp>
#include <aoba/yaml/YamlTools.hpp>
#include <aoba/utils/AobaRandom.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
void TilesetTerrainStamp::apply(TilemapTerrainLayer *layer, TilemapTerrainBuildContext &context, const QPoint &pos) const {
  if (mSize.isEmpty()) {
    Aoba::log::warn("Size is empty");
    return;
  }

  int fromX = pos.x() + mOffset.x();
  int fromY = pos.y() + mOffset.y();

  int targetIndex = pos.x() + pos.y() * layer->width();

  switch (mCheckerboard) {
    case CHECKERBOARD_HORIZONTAL:
      fromY += fromX / mSize.width();
      break;
    case CHECKERBOARD_VERTICAL:
      fromX += fromY / mSize.height();
      break;

    default:
      break;
  }
  fromX %= mSize.width();
  fromY %= mSize.height();

  int index = 0;
  if (mTiles.count() > 1) {
    AobaRandom random(pos.x() + pos.y() * 5000 + mSeed);
    index = random.random(mTiles.count());
  }
  
  QMapIterator<QString, TilesetTerrainTileList> it(mTiles[index]);
  while (it.hasNext()) {
    it.next();

    if (!context.layers.contains(it.key())) {
      Aoba::log::warn("Unknown layer " + it.key());
      continue;
    }
    
    TilemapTerrainBuildContextLayer *target = context.layers[it.key() + "/" + context.currentLayer->id()];
    if (!target) {
      target = context.layers[it.key()];
    }
    auto &fromTile = it.value()[fromX + fromY * mSize.width()];

    target->tiles[targetIndex] = target->tileset->require(fromTile.originalIndex, true);
    target->tiles[targetIndex].tile.flipX ^= fromTile.flipX;
    target->tiles[targetIndex].tile.flipY ^= fromTile.flipY;
  }
}

// -----------------------------------------------------------------------------
bool TilesetTerrainStamp::load(Tileset *tileset, const YAML::Node &config) {
  QSize size(1, 1);
  if (Yaml::isObject(config["size"])) {
    size = QSize(Yaml::asInt(config["size"]["width"]), Yaml::asInt(config["size"]["height"]));
  }
  if (size.isEmpty()) {
    Aoba::log::warn("Invalid size for stamp");
    return false;
  }

  mCheckerboard = NO_CHECKERBOARD;
  
  if (Yaml::asString(config["checkerboard"]) == "horizontal") {
    mCheckerboard = CHECKERBOARD_HORIZONTAL;
  } else if (Yaml::asString(config["checkerboard"]) == "vertical") {
    mCheckerboard = CHECKERBOARD_VERTICAL;
  }

  mSeed = 0;
  mSize = size;
  mOffset = QPoint(0, 0);
  mTiles.clear();
  for (const YAML::Node &group : config["tiles"]) {
    TilesetTerrainTileRandomGroup result;

    for (const auto &pair : group) {
      QString layerId(pair.first.as<std::string>().c_str());
      TilesetLayer *layer = tileset->layer(tileset->assetSet()->getLayer(layerId));
      if (!layer) {
        Aoba::log::warn("Unknown layer " + layerId + " in tileset");
        return false;
      }

      QVector<TilesetTileRef> tiles;
      for (const auto &tile : pair.second) {
        tiles.push_back(layer->require(Yaml::asInt(tile), true));
      }

      result.insert(layerId, tiles);
    }

    mTiles.push_back(result);
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TilesetTerrainStamp::save(YAML::Emitter &) const {
  Aoba::log::debug("TODO Save TilesetTerrainStamp");
  return false;
}