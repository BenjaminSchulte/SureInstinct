#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameMonospaceFontAssetType.hpp>
#include <aoba/font/MonospaceFont.hpp>
#include <aoba/font/FontRegistry.hpp>

using namespace Maker;
using namespace Aoba;


// -----------------------------------------------------------------------------
Aoba::MonospaceFont *GameMonospaceFontAsset::configure(const GameConfigNode &config) {
  Aoba::MonospaceFontAssetSet *fonts = dynamic_cast<Aoba::MonospaceFontAssetSet*>(mType->assetSet());
  mProject->fonts()->registerFontSet(fonts);

  return new Aoba::MonospaceFont(fonts, mId, config.asAssetPath("fonts"));
}

// -----------------------------------------------------------------------------
MonospaceFontAssetSet *GameMonospaceFontAssetType::configure(const GameConfigNode &config) {
  uint8_t letterWidth = config["letter_width"].asInt(8);
  uint8_t letterHeight = config["letter_height"].asInt(8);

  return new Aoba::MonospaceFontAssetSet(mProject->project(), mId, letterWidth, letterHeight);
}

// -----------------------------------------------------------------------------
GameMonospaceFontAsset *GameMonospaceFontAssetType::create(const QString &id) const {
  return new GameMonospaceFontAsset(this, mProject, id);
}

// -----------------------------------------------------------------------------
