#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameTilemapAssetType.hpp>
#include <maker/game/GameImageAssetType.hpp>
#include <aoba/image/ImageScriptAssetResolver.hpp>

using namespace Maker;
using namespace Aoba;


// -----------------------------------------------------------------------------
Aoba::Image *GameImageAsset::configure(const GameConfigNode &config) {
  Aoba::ImageAssetSet *images = dynamic_cast<Aoba::ImageAssetSet*>(mType->assetSet());
  Aoba::Image *image = new Aoba::Image(images, mId, config.asAssetPath("images"));

  return image;
}


// -----------------------------------------------------------------------------
ImageAssetSet *GameImageAssetType::configure(const GameConfigNode &config) {
  uint8_t headerRomBank = config["header_rom_bank"].asInt(0xC0);

  QString tilemapId = config["tilemap"].asString("");
  GameTilemapAssetType *tilemaps = mProject->assetTypes().get<GameTilemapAssetType>(tilemapId);
  if (!tilemaps) {
    mProject->log().error("Could not find tilemap type " + tilemapId);
    return nullptr;
  }

  TilemapAssetSet *tilemapSet = dynamic_cast<TilemapAssetSet*>(tilemaps->assetSet());
  ImageAssetSet *set = new ImageAssetSet(mProject, tilemapSet, headerRomBank);

  for (const auto &var : config["variants"].asList()) {
    set->addVariant(var["id"].asString());
  }

  return set;
}

// -----------------------------------------------------------------------------
GameImageAsset *GameImageAssetType::create(const QString &id) const {
  return new GameImageAsset(this, mProject, id);
}

// -----------------------------------------------------------------------------
Aoba::ScriptAssetResolver *GameImageAssetType::createAssetResolver() const {
  return new ImageScriptAssetResolver(dynamic_cast<ImageAssetSet*>(mAssetSet));
}

// -----------------------------------------------------------------------------
