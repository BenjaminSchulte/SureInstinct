#include <aoba/log/Log.hpp>
#include <aoba/tilemap/Tilemap.hpp>
#include <aoba/tilemap/TilemapTerrain.hpp>
#include <aoba/tilemap/TilemapTerrainLayer.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/tileset/TilesetTile.hpp>
#include <aoba/tilemap/TilemapWriter.hpp>
#include <aoba/csv/CsvReader.hpp>
#include <aoba/utils/AobaPath.hpp>
#include <yaml-cpp/yaml.h>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool TilemapWriter::save() {
  if (!createWritablePath(mTilemap->path())) {
    Aoba::log::error("Unable to create path for tilemap writing");
    return false;
  }

  if (!writeHeader()) {
    return false;
  }

  if (!writeSwitch("")) {
    return false;
  }

  for (const QString &key : mTilemap->tilemapSwitches().keys()) {
    if (!writeSwitch(key)) {
      return false;
    }
  }
  return true;
}

// -----------------------------------------------------------------------------
bool TilemapWriter::writeSwitch(const QString &id) {
  TilemapSwitch *sw = mTilemap->tilemapSwitch(id);
  QString prefix = id.isEmpty() ? "" : (id + "-");

  for (TilemapAssetSetLayer *layer : mTilemap->assetSet()->layers()) {
    if (!writeLayer(prefix, sw, layer)) {
      return false;
    }
  }

  if (mTilemap->mapData().terrain()) {
    for (TilemapTerrainLayer *layer : sw->terrain()->layers()) {
      if (!writeTerrainLayer(prefix, sw, layer)) {
        return false;
      }
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TilemapWriter::writeHeader() {
  YAML::Emitter root;
  root << YAML::BeginMap;
  root << YAML::Key << "size" << YAML::Value << YAML::Flow;
  root << YAML::BeginMap;
  root << YAML::Key << "width" << YAML::Value << mTilemap->width();
  root << YAML::Key << "height" << YAML::Value << mTilemap->height();
  root << YAML::EndMap;

  root << YAML::Key << "tileset" << YAML::Value << mTilemap->tileset()->id().toStdString();
  if (!mTilemap->tilemapSwitches().keys().isEmpty()) {
    root << YAML::Key << "switches" << YAML::Value << YAML::BeginSeq;
    for (const QString &key : mTilemap->tilemapSwitches().keys()) {
      root << key.toStdString();
    }
    root << YAML::EndSeq;
  }
  root << YAML::EndMap;
  
  QFile outFile(mTilemap->path().filePath("tilemap.yml"));
  if (!outFile.open(QIODevice::WriteOnly)) {
    Aoba::log::error("Failed to open tilemap.yml for writing");
    return false;
  }

  outFile.write(root.c_str(), root.size());

  return true;
}

// -----------------------------------------------------------------------------
bool TilemapWriter::writeLayer(const QString &prefix, TilemapSwitch *target, TilemapAssetSetLayer *layer) {
  TilemapLayer *tilemapLayer = target->layer(layer);

  QStringList rows;
  for (uint16_t y=0; y<tilemapLayer->height(); y++) {
    QStringList row;
    for (uint16_t x=0; x<tilemapLayer->width(); x++) {
      const auto &tile = tilemapLayer->overlayTile(x, y);

      QString filter = QString(tile.tile.flipX ? "X" : "") + (tile.tile.flipY ? "Y" : "");
      row.push_back(filter + QString::number(tile.tile.originalIndex));
    }
    rows.push_back(row.join(','));
  }

  QFile csvFile(mTilemap->path().filePath(prefix + layer->id() + "-tiles.csv"));
  if (!csvFile.open(QIODevice::WriteOnly)) {
    Aoba::log::error("Failed to open tiles file for writing");
    return false;
  }

  QTextStream out(&csvFile);
  out << rows.join('\n');
  return true;
}

// -----------------------------------------------------------------------------
bool TilemapWriter::writeTerrainLayer(const QString &prefix, TilemapSwitch *, TilemapTerrainLayer *layer) {
  QStringList rows;
  for (uint16_t y=0; y<layer->height(); y++) {
    QStringList row;
    for (uint16_t x=0; x<layer->width(); x++) {
      row.push_back(QString::number(layer->tile(x, y)));
    }
    rows.push_back(row.join(','));
  }

  QFile csvFile(mTilemap->path().filePath(prefix + layer->id() + "-terrain.csv"));
  if (!csvFile.open(QIODevice::WriteOnly)) {
    Aoba::log::error("Failed to open tiles file for writing");
    return false;
  }

  QTextStream out(&csvFile);
  out << rows.join('\n');
  return true;
}