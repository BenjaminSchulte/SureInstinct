#include <aoba/log/Log.hpp>
#include <QPainter>
#include <aoba/image/PaletteImageAccessor.hpp>
#include <aoba/tileset/TilesetTile.hpp>
#include <aoba/tilemap/Tilemap.hpp>
#include <aoba/palette/PaletteGroup.hpp>
#include <aoba/palette/Palette.hpp>
#include <aoba/palette/SnesPalette.hpp>
#include <maker/game/GameTilesetAssetType.hpp>
#include <maker/map/MapSceneLayer.hpp>
#include <maker/map/MapSceneLayerList.hpp>
#include <maker/map/MapEditorContext.hpp>
#include <maker/tileset/TileRenderCache.hpp>
#include <maker/GlobalContext.hpp>

// -----------------------------------------------------------------------------
MapEditorContext::MapEditorContext(QObject *parent)
  : TilesetContext(parent)
{
}

// -----------------------------------------------------------------------------
void MapEditorContext::forceRedraw() {
  context().tileRenderCache()->clear();

  emit tilemapLayerChanged();
  emit tilesetChanged();
  emit tilesetLayerChanged();
  emit brushChanged();
  emit redrawRequested();
  emit mTilesetReader.layersChanged();
}
  
// -----------------------------------------------------------------------------
void MapEditorContext::close() {
  mTileset = nullptr;
  mTilesetLayer = nullptr;
  mLayerList = nullptr;
  mEditLayer = nullptr;
  mTerrainLayer = nullptr;
  mTilesetReader.setLayer(nullptr);
  mTileBrush = TileBrush();
  mTerrainBrush = TileBrush();

  emit layerListChanged(nullptr);
  emit editLayerChanged(nullptr);
  emit terrainLayerChanged();
  emit brushChanged();
  emit tilesetChanged();
  emit tilesetLayerChanged();
  emit tilemapLayerChanged();
}

// -----------------------------------------------------------------------------
void MapEditorContext::setTileset(Aoba::Tileset *set) {
  TilesetContext::setTileset(set);

  if (mTilemapLayer && mTileset) {
    setTilesetLayer(mTileset->layer(mTilemapLayer->tilesetLayer()));
  }
}

// -----------------------------------------------------------------------------
void MapEditorContext::setTerrainBrush(int type) {
  mTerrainBrush = TileBrush({type}, QSize(1, 1));
  emit brushChanged();
}

// -----------------------------------------------------------------------------
void MapEditorContext::setTerrainLayer(const QString &layer) {
  if (layer == mTerrainLayer) {
    return;
  }

  mTerrainLayer = layer;
  mTilemapLayer = nullptr;
  emit terrainLayerChanged();
  emit tilemapLayerChanged();
  checkCurrentTool();
}

// -----------------------------------------------------------------------------
void MapEditorContext::setTilemapLayer(Aoba::TilemapAssetSetLayer *layer) {
  if (layer == mTilemapLayer) {
    return;
  }

  mTilemapLayer = layer;
  mTerrainLayer = nullptr;
  emit tilemapLayerChanged();
  emit terrainLayerChanged();
  checkCurrentTool();

  if (mTilemapLayer && mTileset) {
    setTilesetLayer(mTileset->layer(mTilemapLayer->tilesetLayer()));
  }

  if (mEditLayer && mLayerList && mEditLayer->layer() != nullptr && mEditLayer->layer() != layer) {
    for (MapSceneLayer *editLayer : mLayerList->all()) {
      if (editLayer->layer() != layer) {
        continue;
      }

      setEditLayer(editLayer);
      break;
    }
  }
}

// -----------------------------------------------------------------------------
void MapEditorContext::setLayerList(MapSceneLayerList *list) {
  if (list == mLayerList) {
    return;
  }

  mLayerList = list;
  emit layerListChanged(list);
}

// -----------------------------------------------------------------------------
void MapEditorContext::setEditLayer(MapSceneLayer *layer) {
  if (layer == mEditLayer) {
    return;
  }

  if (mEditLayer) {
    mEditLayer->setActive(false);
  }
  mEditLayer = layer;
  if (mEditLayer) {
    mEditLayer->setActive(true);
  }
  emit editLayerChanged(layer);
  if (layer == nullptr) {
    return;
  }

  if (layer->layer()) {
    setTilemapLayer(layer->layer());
  } else if (!layer->terrainLayerId().isEmpty()) {
    setTerrainLayer(layer->terrainLayerId());
  } else if (layer->isObjectLayer()) {
    checkCurrentTool();
  }
}

// -----------------------------------------------------------------------------
void MapEditorContext::checkCurrentTool() {
  if (!mEditLayer) {
    return;
  }

  if (mEditLayer->supportsTool(mTool)) {
    return;
  }

  setTool(mEditLayer->fallbackTool());
}

// -----------------------------------------------------------------------------
void MapEditorContext::setToolWithValidLayer(MapEditTool tool) {
  if (!mEditLayer || mEditLayer->supportsTool(tool)) {
    setTool(tool);
    return;
  }

  for (MapSceneLayer *layer : mLayerList->all()) {
    if (layer->supportsTool(tool)) {
      setEditLayer(layer);
      setTool(tool);
      return;
    }
  }
}

// -----------------------------------------------------------------------------
void MapEditorContext::setTool(MapEditTool tool) {
  if (mTool == tool) {
    return;
  }

  mTool = tool;
  emit toolChanged();
}
