#pragma once 

#include "CollisionMapSceneLayer.hpp"

namespace Rpg {
struct TilesetTileCollision;
class TilesetCollisionModuleInstance;
}

class MoveEventMapSceneLayer : public CollisionMapSceneLayer {
  Q_OBJECT

public:
  //! Constructor
  MoveEventMapSceneLayer(const QString &name, Aoba::TilemapAssetSetLayer *layer, const QString &switchId, Rpg::TilesetCollisionModuleInstance *mod)
    : CollisionMapSceneLayer(name, layer, switchId, mod)
  {
  }

  //! Renders the screen
  void renderScreen(MapSceneLayerRenderInfo &) override {};
  
  //! Renders the overlay
  void renderOverlay(MapSceneLayerRenderInfo &render, QPainter &) override;
};