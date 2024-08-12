#pragma once

#include <aoba/tileset/TileFlagGroupConfig.hpp>
#include "GameAsset.hpp"
#include "GameAssetType.hpp"

namespace Maker {

class GameTileFlagGroupAsset : public GameAsset<Aoba::TileFlagGroup> {
public:
  //! Constructor
  GameTileFlagGroupAsset(const GameAssetType<Aoba::TileFlagGroup> *type, GameProject *project, const QString &id)
    : GameAsset<Aoba::TileFlagGroup>(type, project, id)
  {}

  //! Creates the asset
  Aoba::TileFlagGroup *configure(const GameConfigNode &config) override;
};

class GameTileFlagGroupAssetType : public GameAssetType<Aoba::TileFlagGroup> {
public:
  //! Deconstructor
  ~GameTileFlagGroupAssetType();

  //! Loads the palette
  GameTileFlagGroupAsset *create(const QString &id) const override;

protected:
  //! Loads the type configuration
  Aoba::TileFlagGroupAssetSet *configure(const GameConfigNode &node) override;

  //! Adds parameters to the set
  bool addParameters(Aoba::TileFlagGroupAssetSet *set, const GameConfigNode &list) const;

  //! Adds parameters to the set
  bool addEnumParameter(Aoba::TileFlagGroupAssetSet *set, const QString &id, const GameConfigNode &list) const;

  //! Adds parameters to the set
  bool addBooleanParameter(Aoba::TileFlagGroupAssetSet *set, const QString &id, const GameConfigNode &list) const;

  //! Adds parameters to the set
  bool addGenerators(Aoba::TileFlagGroupAssetSet *set, const GameConfigNode &list) const;

  //! Adds parameters to the set
  bool addDefaultGenerator(Aoba::TileFlagGroupAssetSet *set, const GameConfigNode &config) const;
};

}