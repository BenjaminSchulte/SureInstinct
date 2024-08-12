#include <aoba/log/Log.hpp>
#include <aoba/image/QImageAccessor.hpp>
#include <aoba/image/PaletteImage.hpp>
#include <aoba/image/PaletteImageAccessor.hpp>
#include <aoba/palette/SnesPalette.hpp>
#include <aoba/palette/Palette.hpp>
#include <aoba/palette/PaletteGroup.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/tileset/TilesetTile.hpp>
#include <aoba/tileset/TilesetLoader.hpp>
#include <aoba/tileset/BgTileImageLoader.hpp>
#include <aoba/csv/CsvReader.hpp>
#include <aoba/yaml/YamlTools.hpp>
#include <aoba/utils/StringUtils.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool TilesetLoader::load() {
  if (!loadConfiguration()) {
    return false;
  }

  for (TilesetAssetSetTiles *tiles : mTileset->assetSet()->tiles()) {
    QString paletteFilePath = mTileset->path().filePath(tiles->id() + "-palette.png");
    QString imageFilePath = mTileset->path().filePath(tiles->id() + "-tiles.png");

    //Aoba::log::debug(".. Loading bgtiles" << tiles->id() << "for tileset" << mTileset->id();
    if (!loadBgTiles(tiles, imageFilePath, paletteFilePath)) {
      return false;
    }
  }

  for (TilesetAssetSetLayer *layer : mTileset->assetSet()->layer()) {
    //Aoba::log::debug(".. Loading layer" << layer->id() << "for tileset" << mTileset->id();
    if (!loadLayer(layer)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TilesetLoader::loadConfiguration() {
  YAML::Node config;
  try {
    config = YAML::LoadFile(mTileset->path().filePath("tileset.yml").toStdString().c_str());
  } catch(YAML::BadFile&) {
    return false;
  }

  if (Yaml::isObject(config["animations"])) {
    if (!loadAnimations(config["animations"])) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TilesetLoader::loadAnimations(const YAML::Node &node) {
  for (const auto &value : node) {
    if (!loadAnimation(value.first.as<std::string>().c_str(), value.second)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TilesetLoader::loadAnimation(const QString &id, const YAML::Node &node) {
  QString imageFilePath = mTileset->path().filePath(Yaml::asString(node["image"]));
  QString paletteFilePath = mTileset->path().filePath(Yaml::asString(node["palette"]));
  QImagePtr paletteImage(QImageAccessor::loadImage(paletteFilePath));
  QImagePtr tilesImage(QImageAccessor::loadImage(imageFilePath));
  if (!tilesImage || !paletteImage) {
    Aoba::log::warn("Unable to load image/palette for animation " + id);
    return false;
  }
  SnesPalettePtr palette(SnesPalette::fromImage(paletteImage.get()));
  if (!palette) {
    Aoba::log::warn("Unable to load palette for animation " + id);
    return false;
  }

  TilesetAssetSetTiles *tilesConfig = mTileset->assetSet()->getTiles(Yaml::asString(node["tiles"]));
  if (!tilesConfig) {
    Aoba::log::warn("Unable to find configured tiles for animation " + id);
    return false;
  }

  int numColorsPerPalette = tilesConfig->paletteGroupSet()->paletteSet()->maxNumColors();
  int bgTileWidth = mTileset->assetSet()->bgTileWidth();
  int bgTileHeight = mTileset->assetSet()->bgTileHeight();
  int numFrames = tilesImage->width() / mTileset->assetSet()->tileWidth();
  int numTiles = tilesImage->height() / mTileset->assetSet()->tileHeight();
  QVector<ImageAccessorPtr> images = QImageAccessor(tilesImage).split(QSize(mTileset->assetSet()->tileWidth(), mTileset->assetSet()->tileHeight()));

  TilesetAnimationGroup *group = mTileset->createAnimationGroup(id, numTiles, tilesConfig);
  for (int frame = 0; frame < numFrames; frame++) {
    TilesetAnimationGroupImage &groupImage = group->createImage();

    for (int tile = 0; tile < numTiles; tile++) {
      ImageList subImages(images[frame + tile * numFrames]->split(QSize(bgTileWidth, bgTileHeight)));
      for (auto &subImage : subImages) {
        QVector<int> paletteIndices = subImage->findPalettes(palette, numColorsPerPalette, true);
        if (paletteIndices.isEmpty()) {
          Aoba::log::warn("Could not find palette for subImage in animation " + id + " frame " + QString::number(frame) + " tile " + QString::number(tile));
          return false;
        }

        SnesPaletteView paletteView = palette->subPalette(paletteIndices.first(), numColorsPerPalette, true);
        PaletteImagePtr paletteViewImage = subImage->convertToPaletteImage(paletteView)->toPaletteImage();
        paletteViewImage->overrideNumColors(tilesConfig->paletteGroupSet()->paletteSet()->numSnesColors());
  
        groupImage.tiles().push_back(paletteViewImage);
      }
    }
  }

  for (const auto &frameConfig : node["frames"]) {
    QString action = Yaml::asString(frameConfig["action"]);
    int delay = Yaml::asInt(frameConfig["delay"], 0);
    int frame = Yaml::asInt(frameConfig["column"], -1);

    if ((action.isEmpty() && delay == 0 && frame < 0) || frame >= numFrames) {
      Aoba::log::warn("Frame index out of range for animation " + id + ". Column " + QString::number(frame) + " requested but number of frames is " + QString::number(numFrames));
      return false;
    }

    TilesetAnimationGroupFrame::Command command(TilesetAnimationGroupFrame::NO_COMMAND);
    uint8_t commandParameter = 0;
    if (action == "user_command") {
      command = TilesetAnimationGroupFrame::USER_COMMAND;
      commandParameter = Yaml::asInt(frameConfig["command"]);
      if (commandParameter < 0x81 || commandParameter >= 0xC0) {
        Aoba::log::error("User command ID out of range");
        return false;
      }
    } else if (!action.isEmpty()) {
      Aoba::log::error("Unsupported tileset animation action: " + action);
      return false;
    }

    group->createFrame(frame, delay, command, commandParameter);
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TilesetLoader::loadLayer(TilesetAssetSetLayer *layerConfig) {
  TilesetLayer *layer = mTileset->layer(layerConfig);
  QFile csvFile(mTileset->path().filePath(layerConfig->id() + "-tiles.csv"));
  if (!csvFile.open(QIODevice::ReadOnly)) {
    Aoba::log::warn("Could not open tiles CSV file: " + (layerConfig->id() + "-tiles.csv"));
    return true;
  }

  QTextStream in(&csvFile);
  CsvReader csv(in, true);

  while (csv.nextRow()) {
    if (csv.isEmptyRow()) {
      continue;
    }

    if (createTile(layer, csv) == nullptr) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
TilesetTile *TilesetLoader::createTile(TilesetLayer *layer, CsvReader &csv) {
  TilesetTile *tile = layer->createTile();
  TilesetTiles *tiles = layer->tileset()->tiles(layer->config()->tiles());

  int numBgX = layer->tileset()->assetSet()->numBgTilesX();
  int numBgY = layer->tileset()->assetSet()->numBgTilesY();
  int numBgTilesPerRow = tiles->numBgTilesX();

  tile->setFlipTile(true, false, StringUtils::toInt(csv.get("flip_x_tile"), -1));
  tile->setFlipTile(false, true, StringUtils::toInt(csv.get("flip_y_tile"), -1));
  tile->setFlipTile(true, true, StringUtils::toInt(csv.get("flip_xy_tile"), -1));
  tile->setEditorNote(csv.get("editor_note"));

  for (uint8_t bg=0; bg<layer->config()->numBackgrounds(); bg++) {
    QString postfix;
    if (bg > 0) {
      postfix = QString::number(bg + 1);
    }

    int row = csv.get("row" + postfix).toInt();
    int col = csv.get("col" + postfix).toInt();
    int priority = csv.get("priority" + postfix).toInt();
    QString animation = csv.get("animation" + postfix).toStdString().c_str();
    int animationFrame = csv.get("animationframe" + postfix).toInt();

    for (int y=0; y<numBgY; y++) {
      for (int x=0; x<numBgX; x++) {
        TilesetTilePart &part = tile->tileLayer(bg)->part(x, y);

        int bgTileIndex = ((row * numBgY) + y) * numBgTilesPerRow + col * numBgX + x;
        const TilesetBgTile &bgTile = tiles->bgTiles()[bgTileIndex];

        if (!animation.isEmpty()) {
          if (mTileset->animation(animation) == nullptr) {
            Aoba::log::warn("Tile referred to animation " + animation + " which is not registered in tileset");
            return nullptr;
          }

          if (animationFrame < 0 || animationFrame >= mTileset->animation(animation)->numTiles()) {
            Aoba::log::warn("Animation frame is out of range " + QString::number(animationFrame));
            return nullptr;
          }

          part.setAnimation(animation, animationFrame);
        }

        part.setBgTileIndex(bgTile.image);
        part.setPaletteIndex(bgTile.palette);
        part.setPriority(!!(priority & 1));
        priority >>= 1;
      }
    }
  }

  for (TilesetAssetSetModule *mod : mTileset->assetSet()->modules()) {
    if (!mod->instanceFor(mTileset)->loadTile(layer, tile, csv)) {
      return nullptr;
    }
  }

  return tile;
}

// -----------------------------------------------------------------------------
bool TilesetLoader::loadBgTiles(TilesetAssetSetTiles *tiles, const QString &imageFilePath, const QString &paletteFilePath) {
  QImagePtr paletteImage(QImageAccessor::loadImage(paletteFilePath));
  QImagePtr tilesImage(QImageAccessor::loadImage(imageFilePath));
  if (!tilesImage || !paletteImage) {
    return false;
  }

  return loadBgTiles(tiles, tilesImage, paletteImage);
}

// -----------------------------------------------------------------------------
bool TilesetLoader::loadBgTiles(TilesetAssetSetTiles *tiles, const QImagePtr &tilesImage, const QImagePtr &paletteImage) {
  BgTileImageLoader loader(tiles->paletteGroupSet()->paletteSet(), QSize(mTileset->assetSet()->bgTileWidth(), mTileset->assetSet()->bgTileHeight()));
  if (!loader.add(tilesImage, paletteImage)) {
    return false;
  }
  
  int numBgTilesX = tilesImage->width() / mTileset->assetSet()->bgTileWidth();
  int oldBgTilesX = mTileset->tiles(tiles)->numBgTilesX();
  return mTileset->tiles(tiles)->add(loader.result(), oldBgTilesX == -1 ? numBgTilesX : oldBgTilesX, loader.generatedPalette());
}

