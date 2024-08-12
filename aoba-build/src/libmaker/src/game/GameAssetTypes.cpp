#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameAssetType.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameScripts.hpp>

#include <maker/game/GamePaletteAssetType.hpp>
#include <maker/game/GamePaletteGroupAssetType.hpp>
#include <maker/game/GameTilesetAssetType.hpp>
#include <maker/game/GameTileFlagGroupAssetType.hpp>
#include <maker/game/GameTilemapAssetType.hpp>
#include <maker/game/GameLevelAssetType.hpp>
#include <maker/game/GameSpriteAssetType.hpp>
#include <maker/game/GameSpriteTypeAssetType.hpp>
#include <maker/game/GameSpriteAnimationIndexAssetType.hpp>
#include <maker/game/GameCharacterAssetType.hpp>
#include <maker/game/GameSampleAssetType.hpp>
#include <maker/game/GameImageAssetType.hpp>
#include <maker/game/GameRawImageAssetType.hpp>
#include <maker/game/GameInstrumentAssetType.hpp>
#include <maker/game/GameStreamAssetType.hpp>
#include <maker/game/GameSongAssetType.hpp>
#include <maker/game/GameSfxAssetType.hpp>
#include <maker/game/GameSfxLibraryAssetType.hpp>
#include <maker/game/GameMonospaceFontAssetType.hpp>
#include <maker/game/GameVariableFontAssetType.hpp>
#include <maker/game/GameRawAssetType.hpp>

using namespace Maker;
using namespace Aoba;

// -----------------------------------------------------------------------------
GameAssetTypes::~GameAssetTypes() {
  for (GameAssetType<AbstractAsset> *type : mAssetTypes) {
    delete type;
  }
}

// -----------------------------------------------------------------------------
void GameAssetTypes::registerDefaultAssetTypes() {
  support<Aoba::Sample>("samples", []() -> Maker::GameAssetType<Aoba::Sample>* { return new Maker::GameSampleAssetType(); });
  support<Aoba::Instrument>("instruments", []() -> Maker::GameAssetType<Aoba::Instrument>* { return new Maker::GameInstrumentAssetType(); });
  support<Aoba::Stream>("streams", []() -> Maker::GameAssetType<Aoba::Stream>* { return new Maker::GameStreamAssetType(); });
  support<Aoba::Song>("songs", []() -> Maker::GameAssetType<Aoba::Song>* { return new Maker::GameSongAssetType(); });
  support<Aoba::Sfx>("sfx", []() -> Maker::GameAssetType<Aoba::Sfx>* { return new Maker::GameSfxAssetType(); });
  support<Aoba::SfxLibrary>("sfx_library", []() -> Maker::GameAssetType<Aoba::SfxLibrary>* { return new Maker::GameSfxLibraryAssetType(); });
  support<Aoba::Palette>("palettes", []() -> Maker::GameAssetType<Aoba::Palette>* { return new Maker::GamePaletteAssetType(); });
  support<Aoba::PaletteGroup>("palette_groups", []() -> Maker::GameAssetType<Aoba::PaletteGroup>* { return new Maker::GamePaletteGroupAssetType(); });
  support<Aoba::TileFlagGroup>("tile_flag_groups", []() -> Maker::GameAssetType<Aoba::TileFlagGroup>* { return new Maker::GameTileFlagGroupAssetType(); });
  support<Aoba::Tileset>("tilesets", []() -> Maker::GameAssetType<Aoba::Tileset>* { return new Maker::GameTilesetAssetType(); });
  support<Aoba::Tilemap>("tilemaps", []() -> Maker::GameAssetType<Aoba::Tilemap>* { return new Maker::GameTilemapAssetType(); });
  support<Aoba::BaseImage>("images", []() -> Maker::GameAssetType<Aoba::BaseImage>* { return new Maker::GameImageAssetType(); });
  support<Aoba::BaseImage>("raw_images", []() -> Maker::GameAssetType<Aoba::BaseImage>* { return new Maker::GameRawImageAssetType(); });
  support<Aoba::SpriteAnimationIndex>("sprite_animation_indexes", []() -> Maker::GameAssetType<Aoba::SpriteAnimationIndex>* { return new Maker::GameSpriteAnimationIndexAssetType(); });
  support<Aoba::SpriteType>("sprite_types", []() -> Maker::GameAssetType<Aoba::SpriteType>* { return new Maker::GameSpriteTypeAssetType(); });
  support<Aoba::Sprite>("sprites", []() -> Maker::GameAssetType<Aoba::Sprite>* { return new Maker::GameSpriteAssetType(); });
  support<Aoba::MonospaceFont>("monospace_fonts", []() -> Maker::GameAssetType<Aoba::MonospaceFont>* { return new Maker::GameMonospaceFontAssetType(); });
  support<Aoba::VariableFont>("variable_fonts", []() -> Maker::GameAssetType<Aoba::VariableFont>* { return new Maker::GameVariableFontAssetType(); });
  support<Aoba::Character>("characters", []() -> Maker::GameAssetType<Aoba::Character>* { return new Maker::GameCharacterAssetType(); });
  support<Aoba::Level>("levels", []() -> Maker::GameAssetType<Aoba::Level>* { return new Maker::GameLevelAssetType(); });
  support<Aoba::RawAsset>("raw", []() -> Maker::GameAssetType<Aoba::RawAsset>* { return new Maker::GameRawAssetType(); });
}

// -----------------------------------------------------------------------------
QVector<GameAssetType<Aoba::AbstractAsset>*> GameAssetTypes::getAll(const QString &id) const {
  QVector<GameAssetType<Aoba::AbstractAsset>*> result;
  for (GameAssetType<Aoba::AbstractAsset> *item : mAssetTypesOrder) {
    if (item->typeId() == id) {
      result.push_back(item);
    }
  }
  return result;
}

// -----------------------------------------------------------------------------
bool GameAssetTypes::load() {
  GameConfigReader config(GameConfigReader::fromFile(mProject, mProject->configFile("asset_types.yml")));

  for (const QString &typeId : mTypeOrder) {
    QStringList allKeys = config[typeId].keys();
  
    do {
      QStringList hasDependencyKeys;
      while (!allKeys.isEmpty()) {
        QString id = allKeys.front();
        allKeys.pop_front();

        GameAssetType<AbstractAsset> *type = mConstructors[typeId]();
        auto assetConfig(config[typeId][id]);
        bool skip=false;

        for (const QString &dependsOn : type->dependencies(assetConfig)) {
          if (mAssetTypes.contains(dependsOn)) {
            continue;
          }

          if (!allKeys.contains(dependsOn)) {
            mProject->log().error("Could not find dependency " + dependsOn);
            return false;
          }

          hasDependencyKeys.push_back(id);
          skip = true;
          break;
        }

        if (skip) {
          continue;
        }

        auto task(mProject->log().assetTask("CONF", id, "-"));

        if (!type->setUp(id, mProject, assetConfig)) {
          return task.failed("Could not configure asset type");
        }

        ScriptAssetResolver *resolver = type->assetResolver();
        if (resolver) {
          mProject->scripts().scriptProject().registerAssetResolver(id, resolver);
        }

        mAssetTypes.insert(id, type);
        mAssetTypesOrder.push_back(type);
        task.succeeded();
      }

      allKeys = hasDependencyKeys;
    } while (!allKeys.isEmpty());
  }

  return true;
}

// -----------------------------------------------------------------------------
bool GameAssetTypes::linkTypes(GameLinkResult &result) const {
  for (const auto *type : mAssetTypesOrder) {
    if (!type->linkType(result)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
