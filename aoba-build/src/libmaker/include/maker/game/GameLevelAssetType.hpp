#pragma once

#include <aoba/level/Level.hpp>
#include "GameAsset.hpp"

namespace Aoba {
struct LevelTilemapRenderOrder;
}

namespace Maker {

class GameTilemapAsset;

class GameLevelAsset : public GameAsset<Aoba::Level> {
public:
  //! Constructor
  GameLevelAsset(const GameAssetType<Aoba::Level> *type, GameProject *project, const QString &id)
    : GameAsset<Aoba::Level>(type, project, id)
  {}

  //! Creates the asset
  Aoba::Level *createNew();

  //! Creates the asset
  Aoba::Level *configure(const GameConfigNode &config) override;

  //! Returns the render order
  QVector<Aoba::LevelTilemapRenderOrder> tilemapRenderOrder() const;

  //! Returns the tileset
  GameTilemapAsset *tilemap() const;
};

class GameLevelAssetType : public GameAssetType<Aoba::Level> {
public:
  //! Deconstructor
  ~GameLevelAssetType();

  //! Loads the level
  GameLevelAsset *create(const QString &id) const override;

  //! Loads an asset
  bool load(Aoba::AbstractAsset *asset) const override;

protected:
  //! Loads the type configuration
  Aoba::LevelAssetSet *configure(const GameConfigNode &node) override;

  //! Creates the asset resolver
  Aoba::ScriptAssetResolver *createAssetResolver() const override;

  //! Loads the tilemap component
  bool loadTilemapComponent(Aoba::LevelAssetSet *level, const GameConfigNode &config);
  
  //! Loads the palette component
  bool loadPaletteComponent(Aoba::LevelAssetSet *level, const GameConfigNode &config);
  
  //! Loads the script component
  bool loadScriptComponent(Aoba::LevelAssetSet *level, const GameConfigNode &config);
  
  //! Loads the script component
  bool loadNpcComponent(Aoba::LevelAssetSet *level, const GameConfigNode &config);
  
  //! Loads the script component
  bool loadSwitchComponent(Aoba::LevelAssetSet *level, const GameConfigNode &config);
  
  //! Loads the script component
  bool loadFlagComponent(Aoba::LevelAssetSet *level, const GameConfigNode &config);
  
  //! Loads the script component
  bool loadCustomPropertiesComponent(Aoba::LevelAssetSet *level, const GameConfigNode &config);
  
  //! List of all components
  QVector<Aoba::LevelComponent*> mComponents;
};

}