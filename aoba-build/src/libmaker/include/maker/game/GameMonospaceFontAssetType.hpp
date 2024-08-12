#pragma once

#include <aoba/font/MonospaceFont.hpp>
#include "GameAssetType.hpp"

namespace Maker {

class GameMonospaceFontAsset : public GameAsset<Aoba::MonospaceFont> {
public:
  //! Constructor
  GameMonospaceFontAsset(const GameAssetType<Aoba::MonospaceFont> *type, GameProject *project, const QString &id)
    : GameAsset<Aoba::MonospaceFont>(type, project, id)
  {}

  //! Creates the asset
  Aoba::MonospaceFont *configure(const GameConfigNode &config) override;
};

class GameMonospaceFontAssetType : public GameAssetType<Aoba::MonospaceFont> {
public:
  //! Loads the palette
  GameMonospaceFontAsset *create(const QString &id) const override;

protected:
  //! Loads the type configuration
  Aoba::MonospaceFontAssetSet *configure(const GameConfigNode &node) override;
  
};

}