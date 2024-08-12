#pragma once

#include <QObject>
#include <QBrush>
#include <QMap>
#include <QPixmap>
#include "../tileset/TilesetContext.hpp"

namespace Aoba {
class TilemapAssetSetLayer;
class TilemapTerrainLayer;
}

enum class MapEditTool {
  BRUSH,
  FILL,
  OBJECT
};

class MapSceneLayer;
class MapSceneLayerList;

class MapEditorContext : public TilesetContext {
  Q_OBJECT

public:
  //! Constructor
  explicit MapEditorContext(QObject *parent = nullptr);

  //! Close the context
  void close();

  //! Force redraw
  void forceRedraw();

  //! Sets the tileset
  void setTileset(Aoba::Tileset *set) override;

  //! Sets the layer
  void setTilemapLayer(Aoba::TilemapAssetSetLayer *layer);

  //! Sets the layer
  void setTerrainLayer(const QString &layer);

  //! Sets the terrain brush
  void setTerrainBrush(int type);

  //! Sets the layer list
  void setLayerList(MapSceneLayerList *list);

  //! Sets the edit layer
  void setEditLayer(MapSceneLayer *layer);

  //! Sets the current tool
  void setTool(MapEditTool tool);

  //! Sets the current tool
  void setToolWithValidLayer(MapEditTool tool);

  //! Returns the tool
  inline MapEditTool tool() const { return mTool; }

  //! Returns the tileset
  inline Aoba::TilemapAssetSetLayer *tilemapLayer() const { return mTilemapLayer; }

  //! Returns the terrain layer
  inline const QString &terrainLayer() const { return mTerrainLayer; }

  //! Returns the layer list
  inline MapSceneLayerList *layerList() const { return mLayerList; }

  //! Returns the edit layer
  inline MapSceneLayer *editLayer() const { return mEditLayer; }

  //! Returns the terrain brush
  inline const TileBrush &terrainBrush() const { return mTerrainBrush; }

signals:
  //! The layer has been changed
  void tilemapLayerChanged();

  //! The terrain layer has changed
  void terrainLayerChanged();

  //! The edit layer has been changed
  void editLayerChanged(MapSceneLayer*);

  //! Layer list has been changed
  void layerListChanged(MapSceneLayerList*);

  //! The current edit layer doesn't fit hte tilemap layer
  void invalidEditLayer();
  
  //! The tool has been changed
  void toolChanged();

protected:
  //! Checks whether the current tool is valid
  void checkCurrentTool();

  //! The current layer
  Aoba::TilemapAssetSetLayer *mTilemapLayer = nullptr;

  //! The current layer
  QString mTerrainLayer;

  //! The layer list
  MapSceneLayerList *mLayerList = nullptr;

  //! The current edit layer
  MapSceneLayer *mEditLayer = nullptr;

  //! The current tool
  MapEditTool mTool = MapEditTool::BRUSH;

  //! The terrain brush
  TileBrush mTerrainBrush;
};