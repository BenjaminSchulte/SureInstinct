#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameVariableFontAssetType.hpp>
#include <aoba/font/VariableFontScriptAssetResolver.hpp>
#include <aoba/font/VariableFont.hpp>
#include <aoba/font/FontRegistry.hpp>

using namespace Maker;
using namespace Aoba;


// -----------------------------------------------------------------------------
Aoba::VariableFont *GameVariableFontAsset::configure(const GameConfigNode &config) {
  Aoba::VariableFontAssetSet *fonts = dynamic_cast<Aoba::VariableFontAssetSet*>(mType->assetSet());
  mProject->fonts()->registerFontSet(fonts);

  return new Aoba::VariableFont(fonts, mId, config.asAssetPath("fonts"));
}

// -----------------------------------------------------------------------------
VariableFontAssetSet *GameVariableFontAssetType::configure(const GameConfigNode &) {
  return new Aoba::VariableFontAssetSet(mProject->project(), mId);
}

// -----------------------------------------------------------------------------
GameVariableFontAsset *GameVariableFontAssetType::create(const QString &id) const {
  return new GameVariableFontAsset(this, mProject, id);
}

// -----------------------------------------------------------------------------
Aoba::ScriptAssetResolver *GameVariableFontAssetType::createAssetResolver() const {
  return new VariableFontScriptAssetResolver(dynamic_cast<VariableFontAssetSet*>(mAssetSet));
}

// -----------------------------------------------------------------------------
