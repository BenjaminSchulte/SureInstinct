#pragma once 

#include "MapSceneLayer.hpp"

namespace Rpg {
class TilesetCollisionModuleInstance;
}

class ExitMapSceneLayer : public MapSceneLayer {
  Q_OBJECT

public:
  //! Constructor
  ExitMapSceneLayer(const QString &name, const QString &switchId, Rpg::TilesetCollisionModuleInstance *mod)
    : MapSceneLayer(name), mModule(mod), mSwitchId(switchId)
  {
    setVisible(false);
  }

  //! Renders the screen
  void renderOverlay(MapSceneLayerRenderInfo &render, QPainter &painter) override;

private:
  //! Creates an exit
  void createExit(MapSceneLayerRenderInfo &render, const QPoint &tile, const QPoint &renderPosition);

  //! The module
  Rpg::TilesetCollisionModuleInstance *mModule;

  //! The switch ID
  QString mSwitchId;
};
