#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GamePaletteGroupAssetType.hpp>
#include <maker/game/GameRawImageAssetType.hpp>
#include <aoba/image/RawImage.hpp>
#include <aoba/image/ImageScriptAssetResolver.hpp>

using namespace Maker;
using namespace Aoba;


// -----------------------------------------------------------------------------
Aoba::RawImage *GameRawImageAsset::configure(const GameConfigNode &config) {
  Aoba::RawImageAssetSet *images = dynamic_cast<Aoba::RawImageAssetSet*>(mType->assetSet());
  Aoba::RawImage *image = new Aoba::RawImage(images, mId, config.asAssetPath("images"));

  return image;
}


// -----------------------------------------------------------------------------
RawImageAssetSet *GameRawImageAssetType::configure(const GameConfigNode &config) {
  uint8_t headerRomBank = config["header_rom_bank"].asInt(0xC0);

  QString paletteId = config["palette_group"].asString("");
  GamePaletteGroupAssetType *palettes = mProject->assetTypes().get<GamePaletteGroupAssetType>(paletteId);
  if (!palettes) {
    mProject->log().error("Could not find palette type " + paletteId);
    return nullptr;
  }

  PaletteGroupSet *paletteSet = dynamic_cast<PaletteGroupSet*>(palettes->assetSet());
  QSize tileSize(
    config["bg_tile_size"]["x"].asInt(8),
    config["bg_tile_size"]["y"].asInt(8)
  );
  uint8_t multiplier = config["tile_index_multiplier"].asInt(1);
  RawImageAssetSet *set = new RawImageAssetSet(mProject, headerRomBank, paletteSet, tileSize, multiplier);

  return set;
}

// -----------------------------------------------------------------------------
GameRawImageAsset *GameRawImageAssetType::create(const QString &id) const {
  return new GameRawImageAsset(this, mProject, id);
}

// -----------------------------------------------------------------------------
Aoba::ScriptAssetResolver *GameRawImageAssetType::createAssetResolver() const {
  return new ImageScriptAssetResolver(dynamic_cast<RawImageAssetSet*>(mAssetSet));
}

// -----------------------------------------------------------------------------
