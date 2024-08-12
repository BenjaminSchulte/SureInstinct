#include <aoba/tilemap/Tilemap.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
TilemapAssetSetLayer::TilemapAssetSetLayer(TilemapAssetSet *owner, const QString &id, const QString &name, TilesetAssetSetLayer *tilesetLayer)
  : mOwner(owner)
  , mId(id)
  , mName(name)
  , mTilesetLayer(tilesetLayer)
{
  mIndex = owner->addLayer(this);
}


// -----------------------------------------------------------------------------
int TilemapAssetSet::addLayer(TilemapAssetSetLayer *layer) {
  mLayers.push_back(layer);
  return mLayers.size() - 1;
}

// -----------------------------------------------------------------------------
TilemapAssetSetLayer* TilemapAssetSet::layer(const QString &id) const {
  for (auto *layer : mLayers) {
    if (layer->id() == id) {
      return layer;
    }
  }

  return nullptr;
}
