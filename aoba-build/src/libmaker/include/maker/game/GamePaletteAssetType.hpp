#pragma once

#include <aoba/palette/Palette.hpp>
#include "GameAssetType.hpp"

namespace Maker {

class GamePaletteAsset : public GameAsset<Aoba::Palette> {
public:
  //! Constructor
  GamePaletteAsset(const GameAssetType<Aoba::Palette> *type, GameProject *project, const QString &id)
    : GameAsset<Aoba::Palette>(type, project, id)
  {}

  //! Creates the asset
  Aoba::Palette *configure(const GameConfigNode &config) override;
  
  //! Converts the asset to YAML assets.yml
  bool toYaml(YAML::Emitter &node) const override {
    node << YAML::Flow << YAML::BeginMap << YAML::Key << "from_file" << YAML::Value << mProject->relativeAssetFile(mAsset->file()).toStdString() << YAML::EndMap;
    return true;
  }
};

class GamePaletteAssetType : public GameAssetType<Aoba::Palette> {
public:
  //! Loads the palette
  GamePaletteAsset *create(const QString &id) const override;

protected:
  //! Loads the type configuration
  Aoba::PaletteAssetSet *configure(const GameConfigNode &node) override;

  //! Creates the asset resolver
  Aoba::ScriptAssetResolver *createAssetResolver() const override;
  
};

}