#include <aoba/log/Log.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <aoba/tilemap/TilemapBuilder.hpp>
#include <aoba/tilemap/TilemapLayerBuilder.hpp>
#include <aoba/tilemap/Tilemap.hpp>
#include <aoba/tileset/Tileset.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool TilemapBuilder::build() {
  FMA::linker::LinkerBlock *block = mTilemap->assetSet()->assetLinkerObject().createLinkerBlock(mTilemap->symbolName(), mTilemap->assetSet()->dataBank());

  // Writes the header
  block->write(FMA::symbol::ReferencePtr(new FMA::symbol::SymbolReference(mTilemap->tileset()->assetSymbolName().toStdString())), 2);
  block->writeNumber(mTilemap->width() - 1, 1);
  block->writeNumber(mTilemap->height() - 1, 1);
  block->writeNumber(1, 1); // Screen Mode

  // Builds the layer index
  for (TilemapAssetSetLayer *layer : mTilemap->assetSet()->layers()) {
    block->write(FMA::symbol::ReferencePtr(new FMA::symbol::SymbolReference(layerSymbolName(layer).toStdString())), 2);
  }

  // Builds the layers
  for (TilemapAssetSetLayer *layer : mTilemap->assetSet()->layers()) {
    block->symbol(layerSymbolName(layer).toStdString());

    if (!buildLayer(layer, block)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
QString TilemapBuilder::layerSymbolName(TilemapAssetSetLayer *layer) const {
  return mTilemap->symbolName() + "__layer_" + layer->id();
}

// -----------------------------------------------------------------------------
bool TilemapBuilder::buildLayer(TilemapAssetSetLayer *config, FMA::linker::LinkerBlock *block) {
  TilemapLayer *layer = mTilemap->mapData().layer(config);
  
  TilemapLayerBuilder builder(layer);
  return builder.build(block);
}
