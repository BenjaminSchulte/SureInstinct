#include <aoba/tileset/Tileset.hpp>
#include <aoba/palette/PaletteGroup.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
TilesetAssetSetTiles::TilesetAssetSetTiles(TilesetAssetSet *owner, const QString &id, PaletteGroupSet *palette, int maxTiles)
  : mOwner(owner)
  , mPalette(palette)
  , mId(id)
  , mMaxTiles(maxTiles)
{
  mIndex = owner->addTiles(this);
}

// -----------------------------------------------------------------------------
int TilesetAssetSetTiles::paletteBase() const {
  return mPalette->paletteBase();
}


// -----------------------------------------------------------------------------
TilesetAssetSetLayer::TilesetAssetSetLayer(TilesetAssetSet *owner, const QString &id, TilesetAssetSetTiles *tiles, int maxTiles, TilesetLayerMode mode, uint8_t numBackgrounds)
  : mOwner(owner)
  , mMode(mode)
  , mId(id)
  , mTiles(tiles)
  , mMaxTiles(maxTiles)
  , mNumBackgrounds(numBackgrounds)
{
  mIndex = owner->addLayer(this);
}


// -----------------------------------------------------------------------------
int TilesetAssetSet::addTiles(TilesetAssetSetTiles *layer) {
  mTiles.push_back(layer);
  return mTiles.size() - 1;
}

// -----------------------------------------------------------------------------
int TilesetAssetSet::addLayer(TilesetAssetSetLayer *layer) {
  mLayers.push_back(layer);
  return mLayers.size() - 1;
}

// -----------------------------------------------------------------------------
void TilesetAssetSetModule::destroyAllInstances() {
  for (TilesetAssetSetModuleInstance *instance : mInstances) {
    delete instance;
  }

  mInstances.clear();
}
