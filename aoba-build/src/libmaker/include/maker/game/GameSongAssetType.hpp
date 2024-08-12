#pragma once

#include <aoba/song/Song.hpp>
#include "GameAssetType.hpp"

namespace Maker {

class GameSongAsset : public GameAsset<Aoba::Song> {
public:
  //! Constructor
  GameSongAsset(const GameAssetType<Aoba::Song> *type, GameProject *project, const QString &id)
    : GameAsset<Aoba::Song>(type, project, id)
  {}

  //! Creates the asset
  Aoba::Song *configure(const GameConfigNode &config) override;
  
  //! Converts the asset to YAML assets.yml
  bool toYaml(YAML::Emitter &node) const override {
    node << YAML::Flow << YAML::BeginMap << YAML::Key << "from_file" << YAML::Value << mProject->relativeAssetFile(mAsset->file()).toStdString() << YAML::EndMap;
    return true;
  }
};

class GameSongAssetType : public GameAssetType<Aoba::Song> {
public:
  //! Loads the palette
  GameSongAsset *create(const QString &id) const override;

protected:
  //! Loads the type configuration
  Aoba::SongAssetSet *configure(const GameConfigNode &node) override;

  //! Creates the asset resolver
  Aoba::ScriptAssetResolver *createAssetResolver() const override;
  
};

}