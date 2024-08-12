#pragma once

#include <aoba/stream/Stream.hpp>
#include "GameAssetType.hpp"

namespace Maker {

class GameStreamAsset : public GameAsset<Aoba::Stream> {
public:
  //! Constructor
  GameStreamAsset(const GameAssetType<Aoba::Stream> *type, GameProject *project, const QString &id)
    : GameAsset<Aoba::Stream>(type, project, id)
  {}

  //! Creates the asset
  Aoba::Stream *configure(const GameConfigNode &config) override;
  
  //! Converts the asset to YAML assets.yml
  bool toYaml(YAML::Emitter &node) const override {
    node << YAML::Flow << YAML::BeginMap << YAML::Key << "from_file" << YAML::Value << mProject->relativeAssetFile(mAsset->file()).toStdString() << YAML::EndMap;
    return true;
  }
};

class GameStreamAssetType : public GameAssetType<Aoba::Stream> {
public:
  //! Loads the palette
  GameStreamAsset *create(const QString &id) const override;

protected:
  //! Loads the type configuration
  Aoba::StreamAssetSet *configure(const GameConfigNode &node) override;

  //! Creates the asset resolver
  Aoba::ScriptAssetResolver *createAssetResolver() const override;
  
};

}