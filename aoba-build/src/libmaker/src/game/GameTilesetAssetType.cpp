#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GamePaletteAssetType.hpp>
#include <maker/game/GamePaletteGroupAssetType.hpp>
#include <maker/game/GameTilesetAssetType.hpp>
#include <maker/game/GameTileFlagGroupAssetType.hpp>
#include <rpg/tileset/TilesetCollisionModule.hpp>
#include <aoba/tileset/TileFlagGroupModule.hpp>
#include <aoba/tileset/TileBitplane2dModule.hpp>
#include <aoba/tileset/TilesetAnimationModule.hpp>
#include <aoba/tileset/TilesetTileListModule.hpp>

using namespace Maker;
using namespace Aoba;
using namespace Rpg;

// -----------------------------------------------------------------------------
Aoba::Tileset *GameTilesetAsset::configure(const GameConfigNode &config) {
  Aoba::TilesetAssetSet *tilesets = dynamic_cast<Aoba::TilesetAssetSet*>(mType->assetSet());

  return new Aoba::Tileset(tilesets, mId, config.asAssetPath("tilesets"));
}


// -----------------------------------------------------------------------------
GameTilesetAssetType::~GameTilesetAssetType() {
  for (auto *tiles : mTiles) {
    delete tiles;
  }
  for (auto *layer : mLayers) {
    delete layer;
  }
  for (auto *mod : mModules) {
    delete mod;
  }
}

// -----------------------------------------------------------------------------
TilesetAssetSet *GameTilesetAssetType::configure(const GameConfigNode &config) {
  uint8_t headerRomBank = config["header_rom_bank"].asInt(0xC0);
  uint8_t animationRomBank = config["animation_rom_bank"].asInt(0xC0);
  uint8_t tileWidth = config["tile_size"]["x"].asInt(16);
  uint8_t tileHeight = config["tile_size"]["y"].asInt(16);
  uint8_t bgTileWidth = config["bg_tile_size"]["x"].asInt(8);
  uint8_t bgTileHeight = config["bg_tile_size"]["y"].asInt(8);
  uint8_t multiplier = config["tile_index_multiplier"].asInt(1);

  int32_t maxBytesCompression = config["max_bytes_per_compression_chunk"].asInt(0x2000);

  TilesetAssetSet *set = new TilesetAssetSet(mProject->project(), mId, headerRomBank, animationRomBank, tileWidth, tileHeight, bgTileWidth, bgTileHeight);
  set->setTileIndexMultiplier(multiplier);

  set->setMaxBytesPerCompressionChunk(maxBytesCompression);
  for (const auto &tilesConfig : config["tiles"].asList()) {
    QString tilesName = tilesConfig["id"].asString();

    QString paletteId = tilesConfig["palette_group"].asString("");
    GamePaletteGroupAssetType *palettes = mProject->assetTypes().get<GamePaletteGroupAssetType>(paletteId);
    if (!palettes) {
      mProject->log().error("Could not find palette group type " + paletteId);
      delete set;
      return nullptr;
    }

    PaletteGroupSet *paletteSet = dynamic_cast<PaletteGroupSet*>(palettes->assetSet());
    uint16_t maxTiles = tilesConfig["max_tiles"].asInt(256);
    TilesetAssetSetTiles *tiles = new TilesetAssetSetTiles(set, tilesName, paletteSet, maxTiles);
    mTiles.insert(tilesName, tiles);
  }

  for (const auto &layerConfig : config["layers"].asList()) {
    QString layerName = layerConfig["id"].asString();
    QString tiles = layerConfig["tiles"].asString();
    QString type = layerConfig["type"].asString("tile_index");
    uint16_t maxTiles = layerConfig["max_tiles"].asInt(256);
    uint16_t numBgs = layerConfig["num_bgs"].asInt(1);

    if (!mTiles.contains(tiles)) {
      mProject->log().error("Could not find tileset tiles " + tiles);
      delete set;
      return nullptr;
    }

    TilesetLayerMode mode(TilesetLayerMode::DIRECT);
    if (type == "tile_index") {
      mode = TilesetLayerMode::TILEINDEX;
    } else if (type == "direct") {
    } else {
      mProject->log().warning("Unsupported tileset layer mode " + type);
    }

    TilesetAssetSetLayer *layer = new TilesetAssetSetLayer(set, layerName, mTiles[tiles], maxTiles, mode, numBgs);
    mLayers.insert(layerName, layer);
  }

  for (const GameConfigNode &mod : config["modules"].asList()) {
    QString type(mod["module"].asString());
    
    if (type == "collision" && addCollisionModule(set, mod)) {
    } else if (type == "tile_flag_group" && addTileFlagGroupModule(set, mod)) {
    } else if (type == "tile_bitplane2d" && addTileBitplane2dModule(set, mod)) {
    } else if (type == "tile_list" && addTileListModule(set, mod)) {
    } else if (type == "tileset_animation" && addTilesetAnimationModule(set, mod)) {
    } else {
      mProject->log().error("Failed to add tileset module: " + type);
      delete set;
      return nullptr;
    }
  }

  return set;
}

// -----------------------------------------------------------------------------
bool GameTilesetAssetType::addTileFlagGroupModule(Aoba::TilesetAssetSet *set, const GameConfigNode &config) {
  GameTileFlagGroupAssetType *group = mProject->assetTypes().get<GameTileFlagGroupAssetType>(config["group"].asString());
  if (!group) {
    mProject->log().error("Could not find tile flag group type " + config["group"].asString());
    return false;
  }

  TileFlagGroupAssetSet *groupSet = dynamic_cast<TileFlagGroupAssetSet*>(group->assetSet());
  TileFlagGroupModule *mod = new TileFlagGroupModule(set, groupSet);
  mModules.push_back(mod);
  return true;
}

// -----------------------------------------------------------------------------
bool GameTilesetAssetType::addTilesetAnimationModule(Aoba::TilesetAssetSet *set, const GameConfigNode &config) {
  TilesetAnimationModule *mod = new TilesetAnimationModule(set, config["name"].asString("animation"));
  mModules.push_back(mod);
  return true;
}


// -----------------------------------------------------------------------------
bool GameTilesetAssetType::addTileBitplane2dModule(Aoba::TilesetAssetSet *set, const GameConfigNode &config) {
  TileFlagGroupAssetSet *groupSet = nullptr;
  QString propertiesName = config["properties"].asString();

  if (!propertiesName.isEmpty()) {
    GameTileFlagGroupAssetType *group = mProject->assetTypes().get<GameTileFlagGroupAssetType>(propertiesName);
    if (!group) {
      mProject->log().error("Could not find tile flag group type " + propertiesName);
      return false;
    }

    groupSet = dynamic_cast<TileFlagGroupAssetSet*>(group->assetSet());
  }

  TileBitplane2dModule *mod = new TileBitplane2dModule(
    set,
    config["name"].asString("bitplane"),
    config["max_size_in_bytes"].asInt(0x10000),
    groupSet
  );
  mModules.push_back(mod);
  return true;
}

// -----------------------------------------------------------------------------
bool GameTilesetAssetType::addCollisionModule(Aoba::TilesetAssetSet *set, const GameConfigNode &config) {
  TilesetCollisionModule *mod = new TilesetCollisionModule(set, config["id"].asString());
  mModules.push_back(mod);

  for (const QString &id : config["movements"].keys()) {
    mod->addMovement(id, config["movements"][id].asInt(0));
  }
  for (const QString &id : config["events"].keys()) {
    mod->addEvent(id, config["events"][id].asInt(0));
  }

  return true;
}

// -----------------------------------------------------------------------------
bool GameTilesetAssetType::addTileListModule(Aoba::TilesetAssetSet *set, const GameConfigNode &config) {
  TilesetTileListModule *mod = new TilesetTileListModule(
    set,
    config["name"].asString(),
    config["bytes_per_tile"].asInt(1),
    config["list_rom_bank"].asInt(0xC0)
  );
  mModules.push_back(mod);
  return true;
}

// -----------------------------------------------------------------------------
GameTilesetAsset *GameTilesetAssetType::create(const QString &id) const {
  return new GameTilesetAsset(this, mProject, id);
}

// -----------------------------------------------------------------------------
