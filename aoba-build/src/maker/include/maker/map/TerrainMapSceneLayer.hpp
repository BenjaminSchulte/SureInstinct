#pragma once 

#include "MapSceneLayer.hpp"
#include "MapScreenshotHelper.hpp"

class TerrainMapSceneLayer : public MapSceneLayer {
  Q_OBJECT

public:
  //! Constructor
  TerrainMapSceneLayer(const QString &name, const QString &layerId, const QString &switchId)
    : MapSceneLayer(name), mLayerId(layerId), mSwitchId(switchId) {}

  //! Returns the tilemap layer
  QString terrainLayerId() const override { return mLayerId; }
  
  //! Returns the matching brush
  const TileBrush &brush(const MapScene*) const override;

  //! Returns a tile
  TileBrushTile getTile(const MapScene*, const QPoint &) const override;

  //! Sets a tile
  void setTiles(MapScene*, const PositionedTileList &) const override;

  //! Commits changed
  void commitSetTile(MapScene*) override;

private:
  //! The layer
  QString mLayerId;

  //! The switch ID
  QString mSwitchId;
};
