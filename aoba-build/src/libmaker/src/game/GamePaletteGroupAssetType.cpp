#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GamePaletteAssetType.hpp>
#include <maker/game/GamePaletteGroupAssetType.hpp>
#include <aoba/palette/PaletteGroupAnimationComponent.hpp>
#include <aoba/palette/PaletteGroupScriptAssetResolver.hpp>

using namespace Maker;
using namespace Aoba;


// -----------------------------------------------------------------------------
Aoba::PaletteGroup *GamePaletteGroupAsset::configure(const GameConfigNode &config) {
  Aoba::PaletteGroupSet *groups = dynamic_cast<Aoba::PaletteGroupSet*>(mType->assetSet());
  mFileName = config.asAssetFileName("palettes", "palette.png");

  Aoba::PaletteGroup *group = new Aoba::PaletteGroup(groups, mId);
  mFromRow = config["from_index"].asInt(0);
  mToRow = config["to_index"].asInt(15);
  for (int i=mFromRow; i<=mToRow; i++) {
    group->add(i, mFileName, i);
  }

  return group;
}

// -----------------------------------------------------------------------------
bool GamePaletteGroupAsset::toYaml(YAML::Emitter &node) const {
  node << YAML::Flow << YAML::BeginMap;
  node << YAML::Key << "from_file" << YAML::Value << mFileName.toStdString();
  node << YAML::Key << "from_index" << YAML::Value << mFromRow;
  node << YAML::Key << "to_index" << YAML::Value << mToRow;
  node << YAML::EndMap;
  return true;
}


// -----------------------------------------------------------------------------
PaletteGroupSet *GamePaletteGroupAssetType::configure(const GameConfigNode &config) {
  QString paletteId = config["palette"].asString("");
  GamePaletteAssetType *palettes = mProject->assetTypes().get<GamePaletteAssetType>(paletteId);
  if (!palettes) {
    mProject->log().error("Could not find palette type " + paletteId);
    return nullptr;
  }

  int paletteBase = config["palette_base"].asInt(0);

  PaletteAssetSet *paletteSet = dynamic_cast<PaletteAssetSet*>(palettes->assetSet());
  PaletteGroupSet *set = new PaletteGroupSet(mProject->project(), mId, paletteSet, paletteBase);

  for (const GameConfigNode &comp : config["components"].asList()) {
    QString type(comp["component"].asString());

    if (type == "animation" && loadAnimationComponent(set, comp)) {
    } else {
      mProject->log().error("Could not load palette group component: " + type);
      delete set;
      return nullptr;
    }
  }

  return set;
}

// -----------------------------------------------------------------------------
bool GamePaletteGroupAssetType::loadAnimationComponent(Aoba::PaletteGroupSet *palette, const GameConfigNode &config) {
  Aoba::PaletteGroupComponent *comp = new Aoba::PaletteGroupAnimationComponent(config["id"].asString());
  palette->addComponent(comp);
  mComponents.push_back(comp);
  return true;
}

// -----------------------------------------------------------------------------
GamePaletteGroupAsset *GamePaletteGroupAssetType::create(const QString &id) const {
  return new GamePaletteGroupAsset(this, mProject, id);
}

// -----------------------------------------------------------------------------
Aoba::ScriptAssetResolver *GamePaletteGroupAssetType::createAssetResolver() const {
  return new PaletteGroupScriptAssetResolver(dynamic_cast<PaletteGroupSet*>(mAssetSet));
}

// -----------------------------------------------------------------------------
