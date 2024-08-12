#include <aoba/tileset/Tileset.hpp>
#include <aoba/tileset/TilesetTile.hpp>
#include <aoba/image/ImageCollection.hpp>
#include <aoba/image/PaletteImageAccessor.hpp>

using namespace Aoba;

TilesetTilePart invalidPart;

// ----------------------------------------------------------------------------
TilesetTileLayer::TilesetTileLayer(TilesetTile *tile)
  : mTile(tile)
  , mNumParts(mTile->layer()->tileset()->assetSet()->numBgTiles())
  , mParts(new TilesetTilePart[numParts()])
{
}

// ----------------------------------------------------------------------------
TilesetTileLayer::~TilesetTileLayer() {
  delete[] mParts;
}

// ----------------------------------------------------------------------------
void TilesetTileLayer::copyFrom(const TilesetTileLayer *other) {
  for (unsigned int i=0; i<numParts(); i++) {
    mParts[i] = other->mParts[i];
  }
}

// ----------------------------------------------------------------------------
TilesetTilePart &TilesetTileLayer::part(unsigned int index) {
  if (index >= numParts()) {
    return invalidPart;
  }

  return mParts[index];
}

// ----------------------------------------------------------------------------
TilesetTilePart &TilesetTileLayer::part(unsigned int x, unsigned int y) {
  if (x >= mTile->layer()->tileset()->assetSet()->numBgTilesX() || y >= mTile->layer()->tileset()->assetSet()->numBgTilesY()) {
    return invalidPart;
  }

  return mParts[x + y * mTile->layer()->tileset()->assetSet()->numBgTilesX()];
}

// ----------------------------------------------------------------------------
const TilesetTilePart &TilesetTileLayer::constPart(unsigned int index) const {
  if (index >= numParts()) {
    return invalidPart;
  }

  return mParts[index];
}

// ----------------------------------------------------------------------------
const TilesetTilePart &TilesetTileLayer::constPart(unsigned int x, unsigned int y) const {
  if (x >= mTile->layer()->tileset()->assetSet()->numBgTilesX() || y >= mTile->layer()->tileset()->assetSet()->numBgTilesY()) {
    return invalidPart;
  }

  return mParts[x + y * mTile->layer()->tileset()->assetSet()->numBgTilesX()];
}

// ----------------------------------------------------------------------------
QString TilesetTileLayer::hash(bool flipX, bool flipY) const {
  QStringList fullHash;

  for (unsigned int i=0; i<numParts(); i++) {
    fullHash << constPart(i).hash(mTile, flipX, flipY);
  }

  return fullHash.join('|');
}



// ----------------------------------------------------------------------------
TilesetTile::TilesetTile(TilesetLayer *layer, int originalIndex)
  : mNumLayers(layer->config()->numBackgrounds())
  , mOriginalIndex(originalIndex)
  , mLayer(layer)
  , mLayers(new TilesetTileLayer*[mNumLayers])
{
  mFlipTile[0] = -1;
  mFlipTile[1] = -1;
  mFlipTile[2] = -1;

  for (uint8_t i=0; i<mNumLayers; i++) {
    mLayers[i] = new TilesetTileLayer(this);
  }
}

// ----------------------------------------------------------------------------
TilesetTile::~TilesetTile() {
  for (uint8_t i=0; i<mNumLayers; i++) {
    delete mLayers[i];
  }
  delete[] mLayers;
}

// ----------------------------------------------------------------------------
TilesetTile *TilesetTile::clone(TilesetLayer *parent, int newIndex) const {
  if (newIndex == -1) {
    newIndex = mOriginalIndex;
  }

  TilesetTile *copy = new TilesetTile(parent ? parent : mLayer, mOriginalIndex);
  for (uint8_t i=0; i<mNumLayers; i++) {
    copy->tileLayer(i)->copyFrom(tileLayer(i));
  }
  copy->mFlipTile[0] = mFlipTile[0];
  copy->mFlipTile[1] = mFlipTile[1];
  copy->mFlipTile[2] = mFlipTile[2];
  copy->mEditorNote = mEditorNote;
  return copy;
}

// ----------------------------------------------------------------------------
int TilesetTile::getFlipTile(bool x, bool y) const {
  int index = ((x ? 1 : 0) | (y ? 2 : 0)) - 1;
  if (index < 0) { return -1; }
  return mFlipTile[index];
}

// ----------------------------------------------------------------------------
void TilesetTile::setFlipTile(bool x, bool y, int tile) {
  int index = ((x ? 1 : 0) | (y ? 2 : 0)) - 1;
  if (index < 0) { return; }
  mFlipTile[index] = tile;
}

// ----------------------------------------------------------------------------
QString TilesetTile::hash(bool flipX, bool flipY) const {
  QStringList fullHash;

  for (unsigned int i=0; i<mNumLayers; i++) {
    fullHash << mLayers[i]->hash(flipX, flipY);
  }

  for (TilesetAssetSetModule *mod : mLayer->tileset()->assetSet()->modules()) {
    fullHash << mod->instanceFor(mLayer->tileset())->hashTile(mLayer, this);
  }

  return fullHash.join('#');
}

// ----------------------------------------------------------------------------
QString TilesetTilePart::hash(const TilesetTile *tile, bool flipX, bool flipY) const {
  auto *tiles = tile->layer()->tileset()->tiles(tile->layer()->config()->tiles());
  ImageAccessorPtr img(new PaletteImageAccessor(tiles->bgTiles()[mBgTileIndex].data));
  return img->flip(flipX, flipY)->hash() + (mPriority ? "P" : "p") + mPaletteIndex; 
}