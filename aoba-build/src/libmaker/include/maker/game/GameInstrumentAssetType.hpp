#pragma once

#include <aoba/instrument/Instrument.hpp>
#include "GameAssetType.hpp"

namespace Maker {

class GameInstrumentAsset : public GameAsset<Aoba::Instrument> {
public:
  //! Constructor
  GameInstrumentAsset(const GameAssetType<Aoba::Instrument> *type, GameProject *project, const QString &id)
    : GameAsset<Aoba::Instrument>(type, project, id)
  {}

  //! Creates the asset
  Aoba::Instrument *configure(const GameConfigNode &config) override;
  
  //! Converts the asset to YAML assets.yml
  bool toYaml(YAML::Emitter &node) const override {
    node << YAML::Flow << YAML::BeginMap << YAML::Key << "from_file" << YAML::Value << mProject->relativeAssetFile(mAsset->file()).toStdString() << YAML::EndMap;
    return true;
  }
};

class GameInstrumentAssetType : public GameAssetType<Aoba::Instrument> {
public:
  //! Loads the palette
  GameInstrumentAsset *create(const QString &id) const override;

protected:
  //! Loads the type configuration
  Aoba::InstrumentAssetSet *configure(const GameConfigNode &node) override;
  
};

}