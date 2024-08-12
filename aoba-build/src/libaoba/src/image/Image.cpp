#include <QDebug>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <aoba/image/Image.hpp>
#include <aoba/image/QImageAccessor.hpp>
#include <aoba/palette/SnesPalette.hpp>
#include <aoba/palette/Palette.hpp>
#include <aoba/palette/PaletteGroup.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/tileset/TilesetTile.hpp>
#include <aoba/tileset/TilesetLoader.hpp>
#include <aoba/tilemap/Tilemap.hpp>
#include <aoba/asset/AssetLinkerObject.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/yaml/YamlTools.hpp>
#include <yaml-cpp/yaml.h>



#include <aoba/image/PaletteImageAccessor.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
BaseImage::BaseImage(BaseImageAssetSet *set, const QString &id)
  : Asset(set, id)
{
}


// -----------------------------------------------------------------------------
Image::Image(ImageAssetSet *set, const QString &id, const QDir &dir)
  : BaseImage(set, id)
  , mPath(dir)
{
  set->add(this);
}

// -----------------------------------------------------------------------------
Image::~Image() {
  for (Tilemap *tilemap : mTilemaps) {
    delete tilemap;
  }
  delete mTileset;
}

// -----------------------------------------------------------------------------
ImageAssetSet *Image::imageAssetSet() const {
  return dynamic_cast<ImageAssetSet*>(mAssetSet);
}

// -----------------------------------------------------------------------------
QString Image::assetSymbolName() const {
  return "__asset_image_" + mId;
}

// -----------------------------------------------------------------------------
bool Image::reload() {
  YAML::Node config;
  try {
    config = YAML::LoadFile(mPath.filePath("image.yml").toStdString().c_str());
  } catch(YAML::BadFile&) {
    return false;
  }

  mTileset = new CustomTileset(imageAssetSet()->tileset(), mId + "_tileset");
  mTileset->tiles(imageAssetSet()->tileset()->tiles().front())->setTileOffset(Yaml::asInt(config["relative_tile_index"], 0));

  if (Yaml::isArray(config["custom_variants"])) {
    for (const auto &node : config["custom_variants"]) {
      mCustomVariants.push_back(Yaml::asString(node));
    }
  }
  
  for (const QString &_variant : allVariants()) {
    std::string variant(_variant.toStdString());
    if (!Yaml::isObject(config["variants"][variant])) {
      continue;
    }

    YAML::Node node = config["variants"][variant];
    if (!addVariant(_variant, Yaml::asString(node["image"]), Yaml::asString(node["palette"]), Yaml::asString(node["priority"]))) {
      return false;
    }
  }
  
  return true;
}

// -----------------------------------------------------------------------------
QStringList Image::allVariants() const {
  QStringList list = imageAssetSet()->variants();
  list.append(mCustomVariants);
  return list;
}

// -----------------------------------------------------------------------------
bool Image::addVariant(const QString &id, const QString &imageFileName, const QString &paletteFileName, const QString &priorityFileName) {
  TilesetLoader loader(mTileset);
  TilesetAssetSetTiles *tiles = imageAssetSet()->tileset()->tiles().front();
  
  Aoba::log::debug("Adding image variant " + id);

  QImagePtr paletteImage(QImageAccessor::loadImage(mPath.filePath(paletteFileName)));
  QImagePtr tilesImage(QImageAccessor::loadImage(mPath.filePath(imageFileName)));
  QImagePtr priorityImage(QImageAccessor::loadImage(mPath.filePath(priorityFileName)));
  if (!tilesImage || !paletteImage) {
    Aoba::log::error("Failed to either load palette or image.");
    return false;
  }

  QImageAccessor priorityImageData(priorityImage);

  TilesetTiles *assetTiles = mTileset->tiles(tiles);
  int baseIndex = assetTiles->bgTiles().size();
  if (!loader.loadBgTiles(tiles, tilesImage, paletteImage)) {
    return false;
  }

  int bgTileWidth  = mTileset->assetSet()->bgTileWidth();
  int bgTileHeight = mTileset->assetSet()->bgTileHeight();
  int numBgTilesX  = tilesImage->width()  / bgTileWidth;
  int numBgTilesY  = tilesImage->height() / bgTileHeight;

  Tilemap *tilemap = new CustomTilemap(imageAssetSet()->tilemap(), mId + "_tiles_" + id, mTileset, numBgTilesX, numBgTilesY);
  mTilemaps.insert(id, tilemap);

  TilesetLayer *tilesetLayer = mTileset->layer(imageAssetSet()->tileset()->layer()[0]);
  TilemapLayer *tilemapLayer = tilemap->mapData().layer(imageAssetSet()->tilemap()->layers()[0]);

  QVector<TilemapTile> tilemapTiles;
  int index = baseIndex;
  for (int y=0; y<numBgTilesY; y++) {
    for (int x= 0; x<numBgTilesX; x++, index++) {
      const TilesetBgTile &bgTile = assetTiles->bgTiles()[index];
      TilesetTile *tile = tilesetLayer->createTile();
      TilesetTilePart &part = tile->tileLayer(0)->part(0, 0);

      bool priority = false;
      if (priorityImage && priorityImageData.pixelColor(x * bgTileWidth, y * bgTileHeight).lightnessF() >= 0.5) {
        priority = true;
      }

      part.setBgTileIndex(bgTile.image);
      part.setPaletteIndex(bgTile.palette);
      part.setPriority(priority);

      tilemapTiles.push_back(tilesetLayer->require(index, true));
    }
  }

  tilemapLayer->setTiles(tilemapTiles, numBgTilesX, numBgTilesY);

  return true;
}

// -----------------------------------------------------------------------------
bool Image::build() {
  FMA::linker::LinkerBlock *block = imageAssetSet()->assetLinkerObject().createLinkerBlock(assetSymbolName(), assetSet()->headerRomBank());
  block->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(mTileset->assetSymbolName().toStdString())), 3);

  for (const QString &variant : allVariants()) {
    if (!mTilemaps.contains(variant)) {
      block->writeNumber(0, 3);
      continue;
    }

    block->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(mTilemaps[variant]->symbolName().toStdString())), 3);
  }

  return true;
}

// -----------------------------------------------------------------------------
QString Image::getFmaCode() const {
  return "";
}

// -----------------------------------------------------------------------------
QStringList Image::getFmaObjectFiles() const {
  return imageAssetSet()->assetLinkerObject().getFmaObjectFiles();
}