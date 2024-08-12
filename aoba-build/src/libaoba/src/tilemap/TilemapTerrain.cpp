#include <QDebug>
#include <aoba/log/Log.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/tileset/TilesetTerrain.hpp>
#include <aoba/tilemap/Tilemap.hpp>
#include <aoba/tilemap/TilemapTerrain.hpp>
#include <aoba/tilemap/TilemapTerrainLayer.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
TilemapTerrain::~TilemapTerrain() {
  clear();
}

// -----------------------------------------------------------------------------
Tilemap *TilemapTerrain::tilemap() const {
  return mTilemapSwitch->tilemap();
}

// -----------------------------------------------------------------------------
void TilemapTerrain::clear() {
  for (TilemapTerrainLayer *layer : mLayer) {
    delete layer;
  }

  mLayer.clear();
}

// -----------------------------------------------------------------------------
bool TilemapTerrain::resize(int left, int top, int right, int bottom) {
  for (TilemapTerrainLayer *layer : mLayer) {
    if (!layer->resize(left, top, right, bottom)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
TilemapTerrainLayer *TilemapTerrain::getOrCreateLayer(const QString &id) {
  if (!mLayer.contains(id)) {
    mLayer.insert(id, new TilemapTerrainLayer(this, id));

    mLayer[id]->resize(0, 0, tilemap()->width(), tilemap()->height());
  }
  
  return mLayer[id];
}

// -----------------------------------------------------------------------------
void TilemapTerrain::build(const QRect &updateRect) {
  TilemapTerrainBuildContext context;
  context.updateRect = QRect(
    QPoint(
      qMax(0, updateRect.left() - 3),
      qMax(0, updateRect.top() - 3)
    ), QPoint(
      qMin(tilemap()->width() - 1, updateRect.right() + 3),
      qMin(tilemap()->height() - 1, updateRect.bottom() + 3)
    )
  ).intersected(QRect(QPoint(0, 0), QSize(tilemap()->width(), tilemap()->height())));

  // Checks if there is a valid terrain
  context.tilesetTerrain = tilemap()->tileset()->terrain();
  if (!context.tilesetTerrain) {
    return;
  }

  // Prepares all layers
  for (TilemapAssetSetLayer *layerConfig : tilemap()->assetSet()->layers()) {
    QString tilesetLayer = layerConfig->tilesetLayer()->id();

    TilemapLayer *layer = mTilemapSwitch->layer(layerConfig);
    context.layers[tilesetLayer + "/" + layerConfig->id()] = new TilemapTerrainBuildContextLayer();
    context.layers[tilesetLayer] = context.layers[tilesetLayer + "/" + layerConfig->id()];
    context.layerPointers.push_back(context.layers[tilesetLayer]);

    auto *record = context.layers[tilesetLayer];
    record->target = layer;
    record->tileset = tilemap()->tileset()->layer(layerConfig->tilesetLayer());
    record->tiles.resize(layer->width() * layer->height());
    record->tiles.fill(record->tileset->require(0, true));
  }

  // Prepares our terrain layers
  for (TilemapTerrainLayer *layer : mLayer) {
    context.terrainLayers[layer].stopped.resize(layer->width() * layer->height());
    context.terrainLayers[layer].tags.resizeAndClear(QSize(layer->width(), layer->height()));
    context.terrainLayers[layer].stopped.fill(false);
    context.terrainLayers[layer].terrainLayer = context.tilesetTerrain->layer(layer->id());
  }
  
  // Builds the terrain
  for (TilemapTerrainLayer *layer : mLayer) {
    if (context.terrainLayers[layer].terrainLayer == nullptr) {
      continue;
    }

    context.currentLayer = layer;
    layer->build(context);
  }

  // Updates the tiles
  for (const auto &layer : context.layers) {
    layer->target->requireTerrainTileArray();

    for (int y=context.updateRect.top(); y<=context.updateRect.bottom(); y++) {
      int rowIndex = y * layer->target->width();
      int index = rowIndex + context.updateRect.left();

      for (int x=context.updateRect.left(); x<=context.updateRect.right(); x++, index++) {
        layer->target->setTerrainTile(index, layer->tiles[index]);
      }
    }
  }

  for (auto *layer : context.layerPointers) {
    delete layer;
  }
}
