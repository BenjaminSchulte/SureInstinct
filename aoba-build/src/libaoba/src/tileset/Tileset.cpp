#include <aoba/image/ImageCollection.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/tileset/TilesetTerrain.hpp>
#include <aoba/tileset/TilesetTile.hpp>
#include <aoba/tileset/TilesetLoader.hpp>
#include <aoba/tileset/TilesetWriter.hpp>
#include <aoba/tileset/TilesetBuilder.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
QString TilesetTiles::imageBinaryFileName() const {
  return binaryFileName("image.img");
}

// -----------------------------------------------------------------------------
QString TilesetTiles::binaryFileName(const QString &of) const {
  return mTileset->binaryFileName("tiles_" + mConfig->id() + "_" + of);
}

// -----------------------------------------------------------------------------
bool TilesetTiles::add(const QVector<TilesetBgTile> &tiles, int numX, const SnesPalettePtr &palette) {
  if (mNumBgTilesX != -1 && numX != mNumBgTilesX) {
    Aoba::log::warn("Number of tiles on X axis must be equal");
    return false;
  }

  int indexOffset = mBgTiles.size();
  for (TilesetBgTile copy : tiles) {
    copy.image += indexOffset;
    mBgTiles.push_back(copy); 
  }
  mNumBgTilesX = numX;
  mGeneratedPalette = palette;
  return true;
}

// -----------------------------------------------------------------------------
int TilesetTiles::numTilesX() const {
  return numBgTilesX() / mTileset->assetSet()->numBgTilesX();
}


// -----------------------------------------------------------------------------
int TilesetTiles::numTilesY() const {
  int numTiles = mBgTiles.count() / mTileset->assetSet()->numBgTilesX() / mTileset->assetSet()->numBgTilesY();
  int tilesX = numTilesX();
  return (numTiles + tilesX - 1) / tilesX;
}


// -----------------------------------------------------------------------------
TilesetLayer::~TilesetLayer() {
  for (TilesetTile *tile : mTiles) {
    delete tile;
  }
}

// -----------------------------------------------------------------------------
TilesetTileRef TilesetLayer::require(int index, bool allowFlip) {
  if (index >= mTiles.length()) {
    Aoba::log::warn("Tile is out of range " + QString::number(index));
    return TilesetTileRef();
  }

  QString tilesCacheHash = QString::number(index) + (allowFlip ? "F" : "");
  if (!mCachedTiles.contains(tilesCacheHash)) {
    mCachedTiles.insert(tilesCacheHash, requireWithoutCache(index, allowFlip));
  }

  return mCachedTiles[tilesCacheHash];
}

// -----------------------------------------------------------------------------
TilesetTileRef TilesetLayer::requireWithoutCache(int index, bool allowFlip) {
  TilesetTile *tile = mTiles[index];

  QString hash00 = tile->hash(false, false);
  if (mUsedTilesCache.contains(hash00)) {
    return mUsedTilesCache[hash00].withOriginalIndex(index);
  }

  if (allowFlip) {
    QString hash10 = tile->hash(true, false);
    if (mUsedTilesCache.contains(hash10)) {
      return mUsedTilesCache[hash10].withFlip(true, false).withOriginalIndex(index);
    }

    QString hash01 = tile->hash(false, true);
    if (mUsedTilesCache.contains(hash01)) {
      return mUsedTilesCache[hash01].withFlip(false, true).withOriginalIndex(index);
    }

    QString hash11 = tile->hash(true, true);
    if (mUsedTilesCache.contains(hash11)) {
      return mUsedTilesCache[hash11].withFlip(true, true).withOriginalIndex(index);
    }
  }

  int newIndex = mUsedTiles.length();
  TilesetTileRef newRef(newIndex, index, false, false);

  mUsedTilesCache.insert(hash00, newRef);
  mUsedTiles.push_back(tile);
  return newRef;
}

// -----------------------------------------------------------------------------
QString TilesetLayer::indexBinaryFileName() const {
  return binaryFileName("index.bin");
}

// -----------------------------------------------------------------------------
QString TilesetLayer::binaryFileName(const QString &of) const {
  return mTileset->binaryFileName("layer_" + mConfig->id() + "_" + of);
}


// -----------------------------------------------------------------------------
void TilesetLayer::clear() {
  mUsedTiles.clear();
  mUsedTilesCache.clear();
  mTiles.clear();
  mCachedTiles.clear();
}

// -----------------------------------------------------------------------------
TilesetTile *TilesetLayer::createTile() {
  TilesetTile *tile = new TilesetTile(this, mTiles.size());
  mTiles.push_back(tile);
  return tile;
}

// -----------------------------------------------------------------------------
TilesetTile *TilesetLayer::createTile(const TilesetTile *other) {
  TilesetTile *tile = other->clone(this, mTiles.size());
  mTiles.push_back(tile);
  return tile;
}

// -----------------------------------------------------------------------------
Tileset::Tileset(TilesetAssetSet *set, const QString &id, const QDir &dir)
  : Asset(set, id)
  , mPath(dir)
{
  set->add(this);
}

// -----------------------------------------------------------------------------
Tileset::~Tileset() {
  for (TilesetTiles *tiles : mTiles) {
    delete tiles;
  }
  for (TilesetLayer *layer : mLayer) {
    delete layer;
  }
  for (TilesetAnimationGroup *animation : mAnimations) {
    delete animation;
  }
  delete mTerrain;
}

// -----------------------------------------------------------------------------
bool Tileset::save() {
  TilesetWriter writer(this);
  return writer.save();
}

// -----------------------------------------------------------------------------
bool Tileset::reload() {
  TilesetLoader loader(this);
  if (!loader.load()) {
    return false;
  }

  YAML::Node terrainConfig;
  try {
    terrainConfig = YAML::LoadFile(mPath.filePath("terrain.yml").toStdString().c_str());

    delete mTerrain;
    mTerrain = new TilesetTerrain(this);
    if (!mTerrain->load(terrainConfig)) {
      return false;
    }
  } catch(YAML::BadFile&) {
  }

  return true;
}

// -----------------------------------------------------------------------------
QString Tileset::assetSymbolName() const {
  return "__asset_tileset_" + mId;
}

// -----------------------------------------------------------------------------
bool Tileset::build() {
  mHeaderBlock = mAssetSet->assetLinkerObject().createLinkerBlock(assetSymbolName(), mAssetSet->headerRomBank());

  TilesetBuilder builder(this);
  if (!builder.build()) {
    return false;
  }

  for (TilesetAssetSetModule *mod : assetSet()->modules()) {
    if (!mod->instanceFor(this)->build()) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
QString Tileset::getFmaCode() const {
  return "";
}

// -----------------------------------------------------------------------------
QStringList Tileset::getFmaObjectFiles() const {
  return mAssetSet->assetLinkerObject().getFmaObjectFiles();
}

// -----------------------------------------------------------------------------
TilesetTiles *Tileset::tiles(TilesetAssetSetTiles *config) {
  if (!mTiles.contains(config->id())) {
    mTiles.insert(config->id(), new TilesetTiles(this, config));
  }

  return mTiles[config->id()];
}

// -----------------------------------------------------------------------------
TilesetLayer *Tileset::layer(TilesetAssetSetLayer *config) {
  if (!mLayer.contains(config->id())) {
    mLayer.insert(config->id(), new TilesetLayer(this, config));
  }

  return mLayer[config->id()];
}

// -----------------------------------------------------------------------------
TilesetAnimationGroup *Tileset::createAnimationGroup(const QString &id, int numTiles, TilesetAssetSetTiles *tiles) {
  if (!mAnimations.contains(id)) {
    mAnimations[id] = new TilesetAnimationGroup(numTiles, tiles);
  }

  return mAnimations[id];
}

// -----------------------------------------------------------------------------
bool CustomTileset::reload() {
  return true;
}
