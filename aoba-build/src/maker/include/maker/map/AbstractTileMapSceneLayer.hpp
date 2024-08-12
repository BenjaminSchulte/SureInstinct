#pragma once 

#include "MapSceneLayer.hpp"
#include "MapScreenshotHelper.hpp"

namespace Aoba {
class TilemapAssetSetLayer;
class TilemapLayer;
}

class AbstractTileMapSceneLayer : public MapSceneLayer {
  Q_OBJECT

public:
  //! Constructor
  AbstractTileMapSceneLayer(const QString &name, Aoba::TilemapAssetSetLayer *layer, const QString &switchId)
    : MapSceneLayer(name), mLayer(layer), mSwitchId(switchId) {}

  //! Returns the tilemap layer
  Aoba::TilemapAssetSetLayer *layer() const override { return mLayer; }

  //! Returns the switch ID
  inline const QString &switchId() const { return mSwitchId; }

  //! Returns the matching brush
  const TileBrush &brush(const MapScene*) const override;

  //! Returns a tile
  TileBrushTile getTile(const MapScene*, const QPoint &) const override;

  //! Sets a tile
  void setTiles(MapScene*, const PositionedTileList &) const override;

  //! Commits changed
  void commitSetTile(MapScene*) override;

protected:
  //! Returns the layer
  Aoba::TilemapLayer *sceneLayer(const MapScene *scene) const;

  //! The layer
  Aoba::TilemapAssetSetLayer *mLayer;

  //! The switch ID
  QString mSwitchId;
};
