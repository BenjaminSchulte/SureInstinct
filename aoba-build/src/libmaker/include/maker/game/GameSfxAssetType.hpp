#pragma once

#include <aoba/sfx/Sfx.hpp>
#include "GameAssetType.hpp"

namespace Maker {

class GameSfxAsset : public GameAsset<Aoba::Sfx> {
public:
  //! Constructor
  GameSfxAsset(const GameAssetType<Aoba::Sfx> *type, GameProject *project, const QString &id)
    : GameAsset<Aoba::Sfx>(type, project, id)
  {}

  //! Creates the asset
  Aoba::Sfx *configure(const GameConfigNode &config) override;
  
  //! Converts the asset to YAML assets.yml
  bool toYaml(YAML::Emitter &node) const override {
    node << YAML::Flow << YAML::BeginMap << YAML::Key << "from_file" << YAML::Value << mProject->relativeAssetFile(mAsset->file()).toStdString() << YAML::EndMap;
    return true;
  }
};

class GameSfxAssetType : public GameAssetType<Aoba::Sfx> {
public:
  //! Loads the palette
  GameSfxAsset *create(const QString &id) const override;

protected:
  //! Loads the type configuration
  Aoba::SfxAssetSet *configure(const GameConfigNode &node) override;

  //! Creates the asset resolver
  Aoba::ScriptAssetResolver *createAssetResolver() const override;
  
};

}