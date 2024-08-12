#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameTilesetAssetType.hpp>
#include <maker/game/GameTilemapAssetType.hpp>

using namespace Maker;
using namespace Aoba;

// -----------------------------------------------------------------------------
Aoba::Tilemap *GameTilemapAsset::createNew(Aoba::Tileset *tileset, const QSize &size) {
  Aoba::TilemapAssetSet *tilemaps = dynamic_cast<Aoba::TilemapAssetSet*>(mType->assetSet());

  mAsset = new Aoba::Tilemap(tilemaps, mId, mProject->assetFile("tilemaps/" + mId));
  mAsset->setTileset(tileset);
  mAsset->setSize(size.width(), size.height());
  return mAsset;
}

// -----------------------------------------------------------------------------
Aoba::Tilemap *GameTilemapAsset::configure(const GameConfigNode &config) {
  Aoba::TilemapAssetSet *tilemaps = dynamic_cast<Aoba::TilemapAssetSet*>(mType->assetSet());

  return new Aoba::Tilemap(tilemaps, mId, config.asAssetPath("tilemaps"));
}

// -----------------------------------------------------------------------------
GameTilesetAsset *GameTilemapAsset::tileset() const {
  return dynamic_cast<GameTilesetAsset*>(mProject->assets().getAsset(mAsset->tileset()->assetSet()->typeId(), mAsset->tileset()->id()));
}


// -----------------------------------------------------------------------------
GameTilemapAssetType::~GameTilemapAssetType() {
  for (auto *layer : mLayers) {
    delete layer;
  }
}

// -----------------------------------------------------------------------------
TilemapAssetSet *GameTilemapAssetType::configure(const GameConfigNode &config) {
  QString tilesetId = config["tileset"].asString("Tileset");
  uint8_t maxWidth = config["max_size"]["x"].asInt(16);
  uint8_t maxHeight = config["max_size"]["y"].asInt(16);
  QString compression = config["compression"].asString("none");

  GameTilesetAssetType *tileset = mProject->assetTypes().get<GameTilesetAssetType>(tilesetId);
  if (!tileset) {
    mProject->log().error("Could not find tileset type " + tilesetId);
    return nullptr;
  }

  TilesetAssetSet *tilesetSet = dynamic_cast<TilesetAssetSet*>(tileset->assetSet());

  TilemapAssetSet *set = new TilemapAssetSet(mProject->project(), mId, tilesetSet, maxWidth, maxHeight, compression);
  set->setDataBank(config["tilemap_rom_bank"].asInt(-1));

  for (const auto &layerConfig : config["layers"].asList()) {
    QString layerId = layerConfig["id"].asString();
    QString layerName = layerConfig["name"].asString(layerId);
    QString tilesetLayerName = layerConfig["tileset_layer"].asString(layerId);

    TilesetAssetSetLayer *tilesetLayer = tilesetSet->getLayer(tilesetLayerName);
    if (!tilesetLayer) {
      mProject->log().error("Could not find tileset layer " + tilesetLayerName);
      delete set;
      return nullptr;
    }

    TilemapAssetSetLayer *layer = new TilemapAssetSetLayer(set, layerId, layerName, tilesetLayer);
    if (Aoba::Yaml::isObject(layerConfig["editor"].yamlNode())) {
      layer->setEditorMirrorX(layerConfig["editor"]["mirror_x"].asBool());
      layer->setEditorMirrorY(layerConfig["editor"]["mirror_y"].asBool());
    }
    mLayers.push_back(layer);
  }

  return set;
}

// -----------------------------------------------------------------------------
GameTilemapAsset *GameTilemapAssetType::create(const QString &id) const {
  return new GameTilemapAsset(this, mProject, id);
}

// -----------------------------------------------------------------------------
