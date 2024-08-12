#pragma once 

#include "AbstractTileMapSceneLayer.hpp"
#include "MapScreenshotHelper.hpp"

namespace Aoba {
class TilemapAssetSetLayer;
}

class TilemapMapSceneLayer : public AbstractTileMapSceneLayer {
  Q_OBJECT

public:
  //! Constructor
  TilemapMapSceneLayer(const QString &name, Aoba::TilemapAssetSetLayer *layer, const QString &switchId, bool front, bool back, bool main, bool sub)
    : AbstractTileMapSceneLayer(name, layer, switchId), mConfiguration(front, back, main, sub) 
  {
    mVisible = switchId.isEmpty();
  }

  //! Renders the screen
  void renderScreen(MapSceneLayerRenderInfo &render) override;

  //! Returns whether front rendering is active (priority)
  inline bool front() const { return mConfiguration.front(); }

  //! Returns whether back rendering is active (non-priority)
  inline bool back() const { return mConfiguration.back(); }

  //! Returns whether this layer in rendered on main screen
  inline bool main() const { return mConfiguration.main(); }

  //! Returns whether this layer in rendered on sub screen
  inline bool sub() const { return mConfiguration.sub(); }

private:
  //! Configuration
  MapScreenshotConfiguration mConfiguration;
};
