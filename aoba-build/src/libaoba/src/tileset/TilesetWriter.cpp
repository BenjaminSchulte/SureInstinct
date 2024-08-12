#include <aoba/log/Log.hpp>
#include <QFile>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/tileset/TilesetTile.hpp>
#include <aoba/tileset/TilesetWriter.hpp>
#include <aoba/csv/CsvWriter.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool TilesetWriter::save() {
  if (!saveConfiguration()) {
    return false;
  }

  for (TilesetAssetSetLayer *layer : mTileset->assetSet()->layer()) {
    //Aoba::log::debug(".. Loading layer" << layer->id() << "for tileset" << mTileset->id();
    if (!saveLayer(layer)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TilesetWriter::saveConfiguration() {
  YAML::Emitter root;

  root << YAML::BeginMap;
  root << YAML::Key << "animations" << YAML::Value << YAML::BeginMap;
  if (!saveAnimations(root)) {
    Aoba::log::warn("Could not save animations");
    return false;
  }
  root << YAML::EndMap << YAML::EndMap;

  QFile file(mTileset->path().filePath("tileset.yml"));
  if (!file.open(QIODevice::WriteOnly)) {
    Aoba::log::warn("Could not open tileset for writing");
    return false;
  }

  file.write(root.c_str(), root.size());

  return true;
}

// -----------------------------------------------------------------------------
bool TilesetWriter::saveAnimations(YAML::Emitter &node) {
  QMapIterator<QString, TilesetAnimationGroup*> it(mTileset->animations());
  while (it.hasNext()) {
    it.next();

    node << YAML::Key << it.key().toStdString() << YAML::Value;
    if (!saveAnimation(node, it.key(), it.value())) {
      Aoba::log::warn("Could not save animation " + it.key());
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TilesetWriter::saveAnimation(YAML::Emitter &node, const QString &id, TilesetAnimationGroup *group) {
  node << YAML::BeginMap;
  node << YAML::Key << "tiles" << YAML::Value << group->tilesConfig()->id().toStdString();
  node << YAML::Key << "palette" << YAML::Value << "main-palette.png";
  node << YAML::Key << "image" << YAML::Value << (id + "-animation.png").toStdString();
  node << YAML::Key << "frames" << YAML::Value << YAML::BeginSeq;
  for (int i=0; i<group->numFrames(); i++) {
    const auto &frame = group->frame(i);
    
    node << YAML::BeginMap;
    if (frame.delay() > 0) {
      node << YAML::Key << "delay" << YAML::Value << (int)frame.delay();
    }
    if (frame.imageIndex() >= 0) {
      node << YAML::Key << "column" << YAML::Value << frame.imageIndex();
    }
    switch (frame.command()) {
      case TilesetAnimationGroupFrame::NO_COMMAND:
        break;
      case TilesetAnimationGroupFrame::USER_COMMAND:
        node << YAML::Key << "action" << YAML::Value << "user_command";
        node << YAML::Key << "command" << YAML::Value << (int)frame.commandParameter();
        break;
    }
    node << YAML::EndMap;
  }
  node << YAML::EndSeq;
  node << YAML::EndMap;

  return true;
}

// -----------------------------------------------------------------------------
bool TilesetWriter::saveLayer(TilesetAssetSetLayer *layerConfig) {
  TilesetLayer *layer = mTileset->layer(layerConfig);
  TilesetTiles *tiles = layer->tileset()->tiles(layer->config()->tiles());

  int numBgX = layer->tileset()->assetSet()->numBgTilesX();
  int numBgY = layer->tileset()->assetSet()->numBgTilesY();
  int numBgTilesPerRow = tiles->numBgTilesX();

  CsvWriter csv;
  for (uint8_t bg=0; bg<layer->config()->numBackgrounds(); bg++) {
    QString postfix;
    if (bg > 0) {
      postfix = QString::number(bg + 1);
    }

    csv << ("row" + postfix) << ("col" + postfix) << ("priority" + postfix) << ("animation" + postfix) << ("animationframe" + postfix);
  }
  for (TilesetAssetSetModule *mod : mTileset->assetSet()->modules()) {
    mod->instanceFor(mTileset)->writeCsvHeader(csv);
  }
  csv << "flip_x_tile" << "flip_y_tile" << "flip_xy_tile" << "editor_note";
  csv.nextRow();

  for (const TilesetTile *fullTile : layer->allTiles()) {
    for (uint8_t bg=0; bg<layer->config()->numBackgrounds(); bg++) {
      const TilesetTileLayer *tile = fullTile->tileLayer(bg);

      int bgTile = tile->constPart(0).bgTileIndex();
      int row = (bgTile / numBgTilesPerRow) / numBgY;
      int col = (bgTile % numBgTilesPerRow) / numBgX;
      int priority = 0;
      for (unsigned int partIndex=0; partIndex<tile->numParts(); partIndex++) {
        if (tile->constPart(partIndex).priority()) {
          priority |= 1 << partIndex;
        }
      }
      csv << row << col << priority;
      if (tile->constPart(0).isAnimatedTile()) {
        csv << tile->constPart(0).animationId();
        csv << tile->constPart(0).animationRow();
      } else {       
        csv << "" << "";
      }
    }

    for (TilesetAssetSetModule *mod : mTileset->assetSet()->modules()) {
      mod->instanceFor(mTileset)->writeTile(layer, fullTile, csv);
    }

    csv << fullTile->getFlipTile(true, false) << fullTile->getFlipTile(false, true) << fullTile->getFlipTile(true, true);
    csv << fullTile->editorNote();

    csv.nextRow();
  }

  QFile csvFile(mTileset->path().filePath(layerConfig->id() + "-tiles.csv"));
  if (!csvFile.open(QIODevice::WriteOnly)) {
    Aoba::log::warn("Could not open layer CSV for writing");
    return false;
  }

  QTextStream out(&csvFile);
  out << csv.toString();

  return true;
}
