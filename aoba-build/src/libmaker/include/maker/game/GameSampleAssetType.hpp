#pragma once

#include <aoba/sample/Sample.hpp>
#include "GameAssetType.hpp"
#include "GameProject.hpp"

namespace Maker {

class GameSampleAsset : public GameAsset<Aoba::Sample> {
public:
  //! Constructor
  GameSampleAsset(const GameAssetType<Aoba::Sample> *type, GameProject *project, const QString &id)
    : GameAsset<Aoba::Sample>(type, project, id)
  {}

  //! Creates the asset
  Aoba::Sample *configure(const GameConfigNode &config) override;
  
  //! Converts the asset to YAML assets.yml
  bool toYaml(YAML::Emitter &node) const override {
    node << YAML::Flow << YAML::BeginMap << YAML::Key << "from_file" << YAML::Value << mProject->relativeAssetFile(mAsset->file()).toStdString() << YAML::EndMap;
    return true;
  }
};

class GameSampleAssetType : public GameAssetType<Aoba::Sample> {
public:
  //! Loads the palette
  GameSampleAsset *create(const QString &id) const override;

protected:
  //! Loads the type configuration
  Aoba::SampleAssetSet *configure(const GameConfigNode &node) override;
  
};

}