#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <aoba/image/SnesPaletteImage.hpp>
#include <aoba/image/RawImage.hpp>
#include <aoba/image/RawImageConfig.hpp>
#include <aoba/image/QImageAccessor.hpp>
#include <aoba/palette/SnesPalette.hpp>
#include <aoba/palette/Palette.hpp>
#include <aoba/palette/PaletteGroup.hpp>
#include <aoba/tileset/BgTileImageLoader.hpp>
#include <aoba/tilemap/Tilemap.hpp>
#include <aoba/asset/AssetLinkerObject.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/yaml/YamlTools.hpp>
#include <yaml-cpp/yaml.h>



#include <aoba/image/PaletteImageAccessor.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
RawImage::RawImage(RawImageAssetSet *set, const QString &id, const QDir &dir)
  : BaseImage(set, id)
  , mPath(dir)
{
  set->add(this);
}

// -----------------------------------------------------------------------------
RawImage::~RawImage() {
}

// -----------------------------------------------------------------------------
RawImageAssetSet *RawImage::imageAssetSet() const {
  return dynamic_cast<RawImageAssetSet*>(mAssetSet);
}

// -----------------------------------------------------------------------------
QString RawImage::assetSymbolName() const {
  return "__asset_rawimage_" + mId;
}

// -----------------------------------------------------------------------------
bool RawImage::reload() {
  YAML::Node config;
  try {
    config = YAML::LoadFile(mPath.filePath("image.yml").toStdString().c_str());
  } catch(YAML::BadFile&) {
    return false;
  }

  mRowsBeforeColumns = Yaml::asBool(config["rows_before_columns"], false);

  for (const QString &_variant : imageAssetSet()->variants()) {
    std::string variant(_variant.toStdString());
    if (!Yaml::isObject(config["variants"][variant])) {
      continue;
    }

    YAML::Node node = config["variants"][variant];
    if (!addVariant(Yaml::asString(node["image"]), Yaml::asString(node["palette"]))) {
      return false;
    }
  }
  
  return true;
}

// -----------------------------------------------------------------------------
bool RawImage::addVariant(const QString &imageFileName, const QString &paletteFileName) {
  //TilesetLoader loader(mTileset);
  //TilesetAssetSetTiles *tiles = imageAssetSet()->tileset()->tiles().front();

  QImagePtr paletteImage(QImageAccessor::loadImage(mPath.filePath(paletteFileName)));
  QImagePtr tilesImage(QImageAccessor::loadImage(mPath.filePath(imageFileName)));
  if (!tilesImage || !paletteImage) {
    Aoba::log::error("Failed to either load palette or image.");
    return false;
  }

  BgTileImageLoader loader(imageAssetSet()->palette()->paletteSet(), imageAssetSet()->tileSize());
  if (!loader.add(tilesImage, paletteImage)) {
    return false;
  }

  if (mRowsBeforeColumns) {
    int numTilesX = tilesImage->width() / imageAssetSet()->tileSize().width();
    int numTilesY = tilesImage->height() / imageAssetSet()->tileSize().height();
    const auto &result = loader.result();

    for (int x=0; x<numTilesX; x++) {
      for (int y=0; y<numTilesY; y++) {
      mImages.push_back(ImageAccessorPtr(new PaletteImageAccessor(result[y * numTilesX + x].data)));
      }
    }

  } else {
    for (const auto &tile : loader.result()) {
      mImages.push_back(ImageAccessorPtr(new PaletteImageAccessor(tile.data)));
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool RawImage::build() {
  FMA::linker::LinkerBlock *block = imageAssetSet()->assetLinkerObject().createLinkerBlock(assetSymbolName());
  PaletteAssetSet *paletteSet = imageAssetSet()->palette()->paletteSet();
  uint8_t colorOffset = paletteSet->colorOffset();

  SnesPaletteImage image(mImages.join()->toPaletteImage());
  QByteArray buffer(image.compile(imageAssetSet()->tileSize().width(), imageAssetSet()->tileSize().height(), colorOffset));
  block->write(buffer.data(), buffer.size());

  return true;
}

// -----------------------------------------------------------------------------
QString RawImage::getFmaCode() const {
  return "";
}

// -----------------------------------------------------------------------------
QStringList RawImage::getFmaObjectFiles() const {
  return imageAssetSet()->assetLinkerObject().getFmaObjectFiles();
}