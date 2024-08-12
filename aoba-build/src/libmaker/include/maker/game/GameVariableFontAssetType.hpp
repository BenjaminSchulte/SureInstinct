#pragma once

#include <aoba/font/VariableFont.hpp>
#include "GameAssetType.hpp"

namespace Maker {

class GameVariableFontAsset : public GameAsset<Aoba::VariableFont> {
public:
  //! Constructor
  GameVariableFontAsset(const GameAssetType<Aoba::VariableFont> *type, GameProject *project, const QString &id)
    : GameAsset<Aoba::VariableFont>(type, project, id)
  {}

  //! Creates the asset
  Aoba::VariableFont *configure(const GameConfigNode &config) override;
};

class GameVariableFontAssetType : public GameAssetType<Aoba::VariableFont> {
public:
  //! Loads the palette
  GameVariableFontAsset *create(const QString &id) const override;

protected:
  //! Loads the type configuration
  Aoba::VariableFontAssetSet *configure(const GameConfigNode &node) override;

  //! Creates the asset resolver
  Aoba::ScriptAssetResolver *createAssetResolver() const override;
  
};

}