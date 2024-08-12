#include <aoba/log/Log.hpp>
#include <aoba/palette/PaletteScriptAssetResolver.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GamePaletteAssetType.hpp>

using namespace Maker;
using namespace Aoba;


// -----------------------------------------------------------------------------
Aoba::Palette *GamePaletteAsset::configure(const GameConfigNode &config) {
  Aoba::PaletteAssetSet *palettes = dynamic_cast<Aoba::PaletteAssetSet*>(mType->assetSet());

  return new Aoba::Palette(palettes, mId, config.asAssetFileName("palettes", "palette.png"), 0);
}


// -----------------------------------------------------------------------------
PaletteAssetSet *GamePaletteAssetType::configure(const GameConfigNode &config) {
  int numColors = config["num_colors"].asInt(16);
  int maxColors = config["max_colors"].asInt(numColors);
  int bank = config["rom_data_bank"].asInt(0xC0);
  int colorOffset = config["color_offset"].asInt(0);
  bool transparence = config["transparence"].asBool(true);
  QString compression = config["compression"].asString("none");

  PaletteAssetSet *set = new PaletteAssetSet(mProject->project(), numColors, maxColors, bank, colorOffset, transparence, compression);
  return set;
}

// -----------------------------------------------------------------------------
GamePaletteAsset *GamePaletteAssetType::create(const QString &id) const {
  return new GamePaletteAsset(this, mProject, id);
}

// -----------------------------------------------------------------------------
Aoba::ScriptAssetResolver *GamePaletteAssetType::createAssetResolver() const {
  return new PaletteScriptAssetResolver(dynamic_cast<PaletteAssetSet*>(mAssetSet));
}

// -----------------------------------------------------------------------------
