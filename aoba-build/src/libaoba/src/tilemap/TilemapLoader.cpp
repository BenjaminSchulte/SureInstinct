#include <aoba/log/Log.hpp>
#include <aoba/tilemap/Tilemap.hpp>
#include <aoba/tilemap/TilemapTerrain.hpp>
#include <aoba/tilemap/TilemapTerrainLayer.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/tileset/TilesetTerrain.hpp>
#include <aoba/tileset/TilesetTerrainLayer.hpp>
#include <aoba/tilemap/TilemapLoader.hpp>
#include <aoba/csv/CsvReader.hpp>
#include <aoba/yaml/YamlTools.hpp>
#include <yaml-cpp/yaml.h>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool TilemapLoader::load() {
  if (!readHeader()) {
    return false;
  }

  for (const QString &id : mSwitchList) {
    if (!readTileAndTerrain(id)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TilemapLoader::readTileAndTerrain(const QString &switchId) {
  QString prefix;
  TilemapSwitch *sw = &mTilemap->mapData();
  
  if (!switchId.isEmpty()) {
    prefix = switchId + "-";
    sw = mTilemap->requireTilemapSwitch(switchId);
  }

  for (TilemapAssetSetLayer *layer : mTilemap->assetSet()->layers()) {
    if (!readLayer(prefix, sw, layer)) {
      return false;
    }
  }

  if (mTilemap->tileset()->terrain()) {
    for (TilesetTerrainLayer *layer : mTilemap->tileset()->terrain()->layers()) {
      if (!readTerrainLayer(prefix, sw, layer)) {
        return false;
      }
    }

    mTilemap->mapData().regenerateTerrain();
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TilemapLoader::readHeader() {
  YAML::Node config;
  try {
    config = YAML::LoadFile(mTilemap->path().filePath("tilemap.yml").toStdString().c_str());
  } catch(YAML::BadFile&) {
    return false;
  }

  // Reads the tileset
  QString tilesetId = QString(config["tileset"].as<std::string>().c_str());
  if (tilesetId.isEmpty()) {
    Aoba::log::error("Failed to load tileset from yaml");
    return false;
  }

  Tileset *tileset = mTilemap->assetSet()->tileset()->get(tilesetId);
  if (!tileset) {
    Aoba::log::error("Could not find tileset " + tilesetId);
    return false;
  }

  mTilemap->setTileset(tileset);
  
  // Reads the size
  int width = config["size"]["width"].as<int>();
  int height = config["size"]["height"].as<int>();
  if (!width || !height) {
    Aoba::log::error("Failed to load size from yaml");
    return false;
  }
  mTilemap->setSize(width, height);

  mSwitchList.clear();
  mSwitchList.push_back("");
  if (Yaml::isArray(config["switches"])) {
    for (const auto &id : config["switches"]) {
      mSwitchList.push_back(id.as<std::string>().c_str());
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TilemapLoader::readLayer(const QString &prefix, TilemapSwitch *target, TilemapAssetSetLayer *layer) {
  TilesetAssetSetLayer *tilesetLayerConfig = layer->tilesetLayer();
  TilesetLayer *tilesetLayer = mTilemap->tileset()->layer(tilesetLayerConfig);
  TilemapLayer *tilemapLayer = target->layer(layer);

  QFile csvFile(mTilemap->path().filePath(prefix + layer->id() + "-tiles.csv"));
  if (!csvFile.open(QIODevice::ReadOnly)) {
    Aoba::log::warn("Could not open tiles CSV file: " + (prefix + layer->id() + "-tiles.csv"));
    QVector<TilemapTile> tiles;
    for (uint16_t y=0; y<mTilemap->height(); y++) {
      for (uint16_t x=0; x<mTilemap->width(); x++) {
        tiles.push_back(tilesetLayer->require(0, true));
      }
    }
    tilemapLayer->setTiles(tiles, mTilemap->width(), mTilemap->height());
    return true;
  }

  QTextStream in(&csvFile);
  CsvReader csv(in);

  QVector<TilemapTile> tiles;
  for (uint16_t y=0; y<mTilemap->height(); y++) {
    if (!csv.nextRow() || csv.numColumns() < mTilemap->width()) {
      Aoba::log::warn("Not enough tiles found in tiles CSV");
      return false;
    }

    for (uint16_t x=0; x<mTilemap->width(); x++) {
      QString indexStr = csv.at(x);

      bool flipX = (indexStr[0] == 'X');
      bool flipY = (indexStr[flipX ? 1 : 0] == 'Y');

      int numberStart = (flipX ? 1 : 0) + (flipY ? 1 : 0);
      int index = QStringRef(&indexStr, numberStart, indexStr.length() - numberStart).toInt();

      TilesetTileRef ref = tilesetLayer->require(index, true);
      ref.flipX ^= flipX;
      ref.flipY ^= flipY;
      tiles.push_back(TilemapTile(ref));
    }
  }

  tilemapLayer->setTiles(tiles, mTilemap->width(), mTilemap->height());

  return true;
}

// -----------------------------------------------------------------------------
bool TilemapLoader::readTerrainLayer(const QString &prefix, TilemapSwitch *target, TilesetTerrainLayer *tilesetLayer) {
  QFile csvFile(mTilemap->path().filePath(prefix + tilesetLayer->id() + "-terrain.csv"));
  if (!csvFile.open(QIODevice::ReadOnly)) {
    return true;
  }

  TilemapTerrainLayer *layer = target->requireTerrain()->getOrCreateLayer(tilesetLayer->id());

  QTextStream in(&csvFile);
  CsvReader csv(in);
  for (uint16_t y=0; y<mTilemap->height(); y++) {
    if (!csv.nextRow() || csv.numColumns() < mTilemap->width()) {
      Aoba::log::warn("Not enough tiles found in tiles CSV");
      return false;
    }

    for (uint16_t x=0; x<mTilemap->width(); x++) {
      layer->setTile(x, y, csv.at(x).toInt());
    }
  }

  return true;
}