#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameCharacterAssetType.hpp>
#include <maker/game/GameLevelAssetType.hpp>
#include <maker/game/GamePaletteGroupAssetType.hpp>
#include <maker/game/GameTilemapAssetType.hpp>
#include <maker/game/GameScripts.hpp>
#include <aoba/level/LevelTilemapComponent.hpp>
#include <aoba/level/LevelExitComponent.hpp>
#include <aoba/level/LevelPaletteGroupComponent.hpp>
#include <aoba/level/LevelScriptComponent.hpp>
#include <aoba/level/LevelSwitchComponent.hpp>
#include <aoba/level/LevelNpcComponent.hpp>
#include <aoba/level/LevelFlagComponent.hpp>
#include <aoba/level/LevelScriptAssetResolver.hpp>
#include <aoba/level/LevelCustomPropertiesComponent.hpp>
#include <aoba/property/GroupPropertyType.hpp>

using namespace Maker;
using namespace Aoba;

// -----------------------------------------------------------------------------
Aoba::Level *GameLevelAsset::createNew() {
  Aoba::LevelAssetSet *levels = dynamic_cast<Aoba::LevelAssetSet*>(mType->assetSet());

  mAsset = new Aoba::Level(levels, mId, mProject->assetFile("levels/" + mId));
  return mAsset;
}

// -----------------------------------------------------------------------------
Aoba::Level *GameLevelAsset::configure(const GameConfigNode &config) {
  Aoba::LevelAssetSet *levels = dynamic_cast<Aoba::LevelAssetSet*>(mType->assetSet());

  return new Aoba::Level(levels, mId, config.asAssetPath("levels"));
}

// -----------------------------------------------------------------------------
QVector<Aoba::LevelTilemapRenderOrder> GameLevelAsset::tilemapRenderOrder() const {
  LevelTilemapComponent *component = mAsset->assetSet()->getComponent<LevelTilemapComponent>();
  if (!component) {
    return {};
  }

  return component->renderOrder();
}

// -----------------------------------------------------------------------------
GameTilemapAsset *GameLevelAsset::tilemap() const {
  LevelTilemapComponent *component = mAsset->assetSet()->getComponent<LevelTilemapComponent>();
  if (!component) {
    return nullptr;
  }

  Aoba::Tilemap *tilemap = component->tilemap(mAsset);
  return dynamic_cast<GameTilemapAsset*>(mProject->assets().getAsset(tilemap->assetSet()->typeId(), tilemap->id()));
}


// -----------------------------------------------------------------------------
GameLevelAssetType::~GameLevelAssetType() {
}

// -----------------------------------------------------------------------------
LevelAssetSet *GameLevelAssetType::configure(const GameConfigNode &config) {
  uint8_t headerRomBank = config["header_rom_bank"].asInt(0xC0);
  LevelAssetSet *set = new LevelAssetSet(mProject->project(), mId, headerRomBank);

  for (const GameConfigNode &comp : config["components"].asList()) {
    QString type(comp["component"].asString());

    if (type == "tilemap" && loadTilemapComponent(set, comp)) {
    } else if (type == "palette_group" && loadPaletteComponent(set, comp)) {
    } else if (type == "scripts" && loadScriptComponent(set, comp)) {
    } else if (type == "npc" && loadNpcComponent(set, comp)) {
    } else if (type == "switches" && loadSwitchComponent(set, comp)) {
    } else if (type == "flags" && loadFlagComponent(set, comp)) {
    } else if (type == "custom_properties" && loadCustomPropertiesComponent(set, comp)) {
    } else if (type == "exit_list") {
      Aoba::LevelComponent *exitList = new Aoba::LevelExitComponent(comp["id"].asString(), comp["data_bank"].asInt(0xC0));
      set->addComponent(exitList);
      mComponents.push_back(exitList);
    } else {
      mProject->log().error("Could not load level component: " + type);
      delete set;
      return nullptr;
    }
  }

  return set;
}

// -----------------------------------------------------------------------------
bool GameLevelAssetType::load(Aoba::AbstractAsset *asset) const {
  if (!GameAssetType<Aoba::Level>::load(asset)) {
    return false;
  }

  Aoba::Level *level = dynamic_cast<Aoba::Level*>(asset);
  for (const QString &script : level->scripts()) {
    mProject->scripts().addScriptFile(script);
  }

  return true;
}

// -----------------------------------------------------------------------------
Aoba::ScriptAssetResolver *GameLevelAssetType::createAssetResolver() const {
  return new LevelScriptAssetResolver(dynamic_cast<LevelAssetSet*>(mAssetSet));
}

// -----------------------------------------------------------------------------
bool GameLevelAssetType::loadTilemapComponent(Aoba::LevelAssetSet *level, const GameConfigNode &config) {
  QString tilemapId = config["tilemap"].asString("");
  GameTilemapAssetType *tilemaps = mProject->assetTypes().get<GameTilemapAssetType>(tilemapId);
  if (!tilemaps) {
    mProject->log().error("Could not find tilemap type " + tilemapId);
    return false;
  }

  TilemapAssetSet *tilemapSet = dynamic_cast<TilemapAssetSet*>(tilemaps->assetSet());

  QVector<Aoba::LevelTilemapRenderOrder> renderOrder;
  for (const auto &order : config["render_order"].asList()) {
    Aoba::LevelTilemapRenderOrder item;
    item.layer = tilemapSet->layer(order["layer"].asString());
    if (!item.layer) {
      mProject->log().error("Could not find layer: " + order["layer"].asString());
      return false;
    }
    item.name = order["name"].asString();
    if (order["priority"].isBoolean()) {
      item.renderPriority = order["priority"].asBool();
      item.renderNonPriority = !item.renderPriority;
    }
    QString target = order["target"].asString();
    if (target == "main") {
      item.sub = false;
    } else if (target == "sub") {
      item.main = false;
    }
    renderOrder.push_back(item);
  }

  Aoba::LevelComponent *comp = new Aoba::LevelTilemapComponent(config["id"].asString(), tilemapSet, renderOrder);
  level->addComponent(comp);
  mComponents.push_back(comp);
  return true;
}

// -----------------------------------------------------------------------------
bool GameLevelAssetType::loadScriptComponent(Aoba::LevelAssetSet *level, const GameConfigNode &config) {
  QStringList scripts;

  for (const auto &node : config["scripts"].asList()) {
    scripts.push_back(node.asString(""));
  }

  Aoba::LevelComponent *comp = new Aoba::LevelScriptComponent(config["id"].asString(), scripts);
  level->addComponent(comp);
  mComponents.push_back(comp);
  return true;
}

// -----------------------------------------------------------------------------
bool GameLevelAssetType::loadNpcComponent(Aoba::LevelAssetSet *level, const GameConfigNode &config) {
  QString characterId = config["character"].asString("");
  GameCharacterAssetType *characters = mProject->assetTypes().get<GameCharacterAssetType>(characterId);
  if (!characters) {
    mProject->log().error("Could not find character type " + characterId);
    return false;
  }

  CharacterAssetSet *characterSet = dynamic_cast<CharacterAssetSet*>(characters->assetSet());

  Aoba::LevelComponent *comp = new Aoba::LevelNpcComponent(config["id"].asString(), characterSet);
  level->addComponent(comp);
  mComponents.push_back(comp);
  return true;
}

// -----------------------------------------------------------------------------
bool GameLevelAssetType::loadSwitchComponent(Aoba::LevelAssetSet *level, const GameConfigNode &config) {
  Aoba::LevelComponent *comp = new Aoba::LevelSwitchComponent(config["id"].asString());
  level->addComponent(comp);
  mComponents.push_back(comp);
  return true;
}


// -----------------------------------------------------------------------------
bool GameLevelAssetType::loadFlagComponent(Aoba::LevelAssetSet *level, const GameConfigNode &config) {
  QVector<LevelFlagScreenMode> screenModes;
  for (const QString &key : config["modes"].keys()) {
    const auto &modeConfig = config["modes"][key];
    screenModes.push_back(LevelFlagScreenMode(modeConfig["id"].asInt(), key));
  }

  Aoba::LevelComponent *comp = new Aoba::LevelFlagComponent(config["id"].asString(), config["default_mode"].asString(), screenModes);
  level->addComponent(comp);
  mComponents.push_back(comp);
  return true;
}

// -----------------------------------------------------------------------------
bool GameLevelAssetType::loadPaletteComponent(Aoba::LevelAssetSet *level, const GameConfigNode &config) {
  QString paletteId = config["palette_group"].asString("");
  GamePaletteGroupAssetType *palettes = mProject->assetTypes().get<GamePaletteGroupAssetType>(paletteId);
  if (!palettes) {
    mProject->log().error("Could not find palette type " + paletteId);
    return false;
  }

  PaletteGroupSet *paletteSet = dynamic_cast<PaletteGroupSet*>(palettes->assetSet());

  Aoba::LevelComponent *comp = new Aoba::LevelPaletteGroupComponent(config["id"].asString(), paletteSet);
  level->addComponent(comp);
  mComponents.push_back(comp);
  return true;
}

// -----------------------------------------------------------------------------
bool GameLevelAssetType::loadCustomPropertiesComponent(Aoba::LevelAssetSet *level, const GameConfigNode &config) {
  GroupPropertyType *group = new GroupPropertyType("root");
  if (!group->configure(config.yamlNode())) {
    delete group;
    return false;
  }

  Aoba::LevelCustomPropertiesComponent *comp = new Aoba::LevelCustomPropertiesComponent(config["id"].asString(), group);
  level->addComponent(comp);
  mComponents.push_back(comp);
  return true;
}

// -----------------------------------------------------------------------------
GameLevelAsset *GameLevelAssetType::create(const QString &id) const {
  return new GameLevelAsset(this, mProject, id);
}

// -----------------------------------------------------------------------------
