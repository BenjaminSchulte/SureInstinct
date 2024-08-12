#pragma once

#include <aoba/palette/PaletteGroup.hpp>
#include "GameAssetType.hpp"

namespace Maker {

class GamePaletteGroupAsset : public GameAsset<Aoba::PaletteGroup> {
public:
  //! Constructor
  GamePaletteGroupAsset(const GameAssetType<Aoba::PaletteGroup> *type, GameProject *project, const QString &id)
    : GameAsset<Aoba::PaletteGroup>(type, project, id)
  {}

  //! Creates the asset
  Aoba::PaletteGroup *configure(const GameConfigNode &config) override;

  //! Converts the data for assets.yml
  bool toYaml(YAML::Emitter &node) const override;

private:
  //! The filename
  QString mFileName;

  //! The first row
  int mFromRow;

  //! The last row
  int mToRow;
};

class GamePaletteGroupAssetType : public GameAssetType<Aoba::PaletteGroup> {
public:
  //! Loads the palette
  GamePaletteGroupAsset *create(const QString &id) const override;

protected:
  //! Loads the type configuration
  Aoba::PaletteGroupSet *configure(const GameConfigNode &node) override;

  //! Creates the asset resolver
  Aoba::ScriptAssetResolver *createAssetResolver() const override;
  
  //! Loads the tilemap component
  bool loadAnimationComponent(Aoba::PaletteGroupSet *palette, const GameConfigNode &config);
  
  //! List of all components
  QVector<Aoba::PaletteGroupComponent*> mComponents;
};

}