#pragma once 

#include "AbstractTileMapSceneLayer.hpp"

namespace Rpg {
struct TilesetTileCollision;
class TilesetCollisionModuleInstance;
}

class CollisionMapSceneLayer : public AbstractTileMapSceneLayer {
  Q_OBJECT

public:
  //! Constructor
  CollisionMapSceneLayer(const QString &name, Aoba::TilemapAssetSetLayer *layer, const QString &switchId, Rpg::TilesetCollisionModuleInstance *mod)
    : AbstractTileMapSceneLayer(name, layer, switchId), mModule(mod)
  {
    setVisible(false);
  }

  //! Renders the screen
  void renderScreen(MapSceneLayerRenderInfo &render) override;

  //! Returns the layer if provided
  Aoba::TilemapAssetSetLayer *layer() const override { return mLayer; } 

protected:
  //! Returns a collision
  Rpg::TilesetTileCollision collisionForTile(MapSceneLayerRenderInfo &render, int x, int y) const;

  //! The module
  Rpg::TilesetCollisionModuleInstance *mModule;
};
