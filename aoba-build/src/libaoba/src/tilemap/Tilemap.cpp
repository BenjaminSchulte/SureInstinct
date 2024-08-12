#include <aoba/image/ImageCollection.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/tileset/TilesetTile.hpp>
#include <aoba/tilemap/Tilemap.hpp>
#include <aoba/tilemap/TilemapTerrain.hpp>
#include <aoba/tilemap/TilemapLoader.hpp>
#include <aoba/tilemap/TilemapWriter.hpp>
#include <aoba/tilemap/TilemapBuilder.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
Tilemap *TilemapLayer::tilemap() const {
  return mTilemapSwitch->tilemap();
}

// -----------------------------------------------------------------------------
TilemapTile TilemapLayer::generatedTile(int x, int y, bool applyLayerMirror) const {
  if (mTerrainTiles.isEmpty()) {
    return overlayTile(x, y);
  }

  if (applyLayerMirror) {
    if (mConfig->editorMirrorX()) { x = mWidth - 1 - x; }
    if (mConfig->editorMirrorY()) { y = mHeight - 1 - y; }
  }

  int index = x + y * mWidth;
  TilemapTile tile;
  if (mTiles[index].tile.originalIndex == 0) {
    tile = mTerrainTiles[index];
  } else {
    tile = mTiles[index];
  }

  if (applyLayerMirror) {
    TilesetTile *tilesetTile = tilemap()->tileset()->layer(mConfig->tilesetLayer())->allTiles()[tile.tile.originalIndex];
    int newIndex = tilesetTile->getFlipTile(mConfig->editorMirrorX(), mConfig->editorMirrorY());
    if (newIndex >= 0) {
      tile.tile.originalIndex = newIndex;
    }
  }

  return tile;
}

// -----------------------------------------------------------------------------
TilemapTile TilemapLayer::terrainTile(int x, int y, bool applyLayerMirror) const {
  if (mTerrainTiles.isEmpty()) {
    return overlayTile(x, y, applyLayerMirror);
  }

  if (applyLayerMirror) {
    if (mConfig->editorMirrorX()) { x = mWidth - 1 - x; }
    if (mConfig->editorMirrorY()) { y = mHeight - 1 - y; }
  }

  return mTerrainTiles[x + y * mWidth];
}

// -----------------------------------------------------------------------------
TilemapTile TilemapLayer::overlayTile(int x, int y, bool applyLayerMirror) const {
  if (applyLayerMirror) {
    if (mConfig->editorMirrorX()) { x = mWidth - 1 - x; }
    if (mConfig->editorMirrorY()) { y = mHeight - 1 - y; }
  }

  TilemapTile tile = mTiles[x + y * mWidth];

  if (applyLayerMirror) {
    TilesetTile *tilesetTile = tilemap()->tileset()->layer(mConfig->tilesetLayer())->allTiles()[tile.tile.originalIndex];
    int newIndex = tilesetTile->getFlipTile(mConfig->editorMirrorX(), mConfig->editorMirrorY());
    if (newIndex >= 0) {
      tile.tile.originalIndex = newIndex;
    }
  }

  return tile;
}

// -----------------------------------------------------------------------------
void TilemapLayer::setTile(int x, int y, const TilemapTile &tile, bool applyLayerMirror) {
  if (applyLayerMirror) {
    if (mConfig->editorMirrorX()) { x = mWidth - 1 - x; }
    if (mConfig->editorMirrorY()) { y = mHeight - 1 - y; }

    TilemapTile copy = tile;
    TilesetTile *tilesetTile = tilemap()->tileset()->layer(mConfig->tilesetLayer())->allTiles()[copy.tile.originalIndex];
    int newIndex = tilesetTile->getFlipTile(mConfig->editorMirrorX(), mConfig->editorMirrorY());
    if (newIndex >= 0) {
      copy.tile.originalIndex = newIndex;
    }
    mTiles[x + y * mWidth] = copy;
  } else {
    mTiles[x + y * mWidth] = tile;
  }
}

// -----------------------------------------------------------------------------
void TilemapLayer::setTiles(const QVector<TilemapTile> &tiles, uint16_t width, uint16_t height) {
  mTiles = tiles;
  mWidth = width;
  mHeight = height;
  mTilemapSwitch->invalidateTerrain(QRect(QPoint(0, 0), QSize(mWidth, mHeight)));
}

// -----------------------------------------------------------------------------
void TilemapLayer::requireTerrainTileArray() {
  int size = mWidth * mHeight;

  if (mTerrainTiles.size() != size) {
    mTerrainTiles.resize(size);
  }
}

// -----------------------------------------------------------------------------
void TilemapLayer::setTerrainTiles(const QVector<TilemapTile> &tiles) {
  mTerrainTiles = tiles;
}

// -----------------------------------------------------------------------------
bool TilemapLayer::resize(int left, int top, int right, int bottom) {
  if (left == 0 && top == 0 && right == 0 && bottom == 0) {
    return true;
  }

  if (!tilemap()->tileset()) {
    Aoba::log::warn("Layer resizeTo() cannot be called when tileset not assigned");
    return false;
  }

  TilemapTile emptyTile(tilemap()->tileset()->layer(config()->tilesetLayer())->require(0, true));

  QVector<TilemapTile> newTiles;
  int newWidth = mWidth + left + right;
  int newHeight = mHeight + top + bottom;

  for (int y=0; y<newHeight; y++) {
    for (int x=0; x<newWidth; x++) {
      int fromX = x - left;
      int fromY = y - top;

      if (fromX >= 0 && fromX < mWidth && fromY >=0 && fromY < mHeight) {
        newTiles.push_back(overlayTile(fromX, fromY));
      } else {
        newTiles.push_back(emptyTile);
      }
    }
  }

  mTiles = newTiles;
  mWidth = newWidth;
  mHeight = newHeight;
  mTilemapSwitch->invalidateTerrain(QRect(QPoint(0, 0), QSize(mWidth, mHeight)));

  return true;
}





// -----------------------------------------------------------------------------
TilemapSwitch::~TilemapSwitch() {
  for (TilemapLayer *layer : mLayers) {
    delete layer;
  }
}

// -----------------------------------------------------------------------------
void TilemapSwitch::invalidateTerrain(const QRect &rect) {
  mInvalidTerrain = mInvalidTerrain.united(rect);
}

// -----------------------------------------------------------------------------
void TilemapSwitch::regenerateTerrain() {
  if (!mInvalidTerrain.isValid()) {
    return;
  }
  
  if (mTerrain) {
    mTerrain->build(mInvalidTerrain);
  }

  mInvalidTerrain = QRect();
}

// -----------------------------------------------------------------------------
TilemapTerrain *TilemapSwitch::requireTerrain() {
  if (!mTerrain) {
    mTerrain = new TilemapTerrain(this);
  }

  return mTerrain;
}

// -----------------------------------------------------------------------------
TilemapLayer *TilemapSwitch::layer(TilemapAssetSetLayer *config) {
  if (!mLayers.contains(config->id())) {
    mLayers.insert(config->id(), new TilemapLayer(this, config));
  }

  return mLayers[config->id()];
}

// -----------------------------------------------------------------------------
bool TilemapSwitch::resize(int left, int top, int right, int bottom) {
  for (auto *layerConfig : tilemap()->assetSet()->layers()) {
    if (!layer(layerConfig)->resize(left, top, right, bottom)) {
      return false;
    }
  }

  if (mTerrain && !mTerrain->resize(left, top, right, bottom)) {
    return false;
  }

  return true;
}



// -----------------------------------------------------------------------------
Tilemap::Tilemap(TilemapAssetSet *set, const QString &id, const QDir &dir)
  : Asset(set, id)
  , mPath(dir)
  , mMainSwitch(this)
{
  set->add(this);
}

// -----------------------------------------------------------------------------
Tilemap::~Tilemap() {
  for (TilemapSwitch *sw : mCustomSwitchList) {
    delete sw;
  }
}

// -----------------------------------------------------------------------------
QString Tilemap::symbolName() const {
  return "__asset_tilemap_" + mId;
}

// -----------------------------------------------------------------------------
void Tilemap::setSize(uint16_t width, uint16_t height) {
  if (!resize(0, 0, (int)width - (int)mWidth, (int)height - (int)mHeight)) {
    Aoba::log::warn("Unable to setSize(" + QString::number(width) + "," + QString::number(height));
  }
}

// -----------------------------------------------------------------------------
bool Tilemap::resize(int left, int top, int right, int bottom) {
  if (!mMainSwitch.resize(left, top, right, bottom)) {
    return false;
  }

  mWidth += left + right;
  mHeight += top + bottom;
  return true;
}

// -----------------------------------------------------------------------------
bool Tilemap::save() {
  TilemapWriter writer(this);
  return writer.save();
}

// -----------------------------------------------------------------------------
bool Tilemap::reload() {
  TilemapLoader loader(this);
  return loader.load();
}

// -----------------------------------------------------------------------------
bool Tilemap::build() {
  TilemapBuilder builder(this);
  if (!builder.build()) {
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
TilemapSwitch *Tilemap::requireTilemapSwitch(const QString &id) {
  if (id.isEmpty()) {
    return &mMainSwitch;
  }

  if (!mCustomSwitchList.contains(id)) {
    mCustomSwitchList.insert(id, new TilemapSwitch(this));
  }

  return mCustomSwitchList[id];
}

// -----------------------------------------------------------------------------
QStringList Tilemap::getFmaObjectFiles() const {
  return mAssetSet->assetLinkerObject().getFmaObjectFiles();
}

// -----------------------------------------------------------------------------
QString Tilemap::getFmaCode() const {
  return "";
}

// -----------------------------------------------------------------------------
CustomTilemap::CustomTilemap(TilemapAssetSet *set, const QString &id, Tileset *tileset, uint16_t width, uint16_t height)
  : Tilemap(set, id, QDir())
{
  mTileset = tileset;
  mWidth = width;
  mHeight = height;
  mIsLoaded = true;
}

// -----------------------------------------------------------------------------
bool CustomTilemap::reload() {
  return true;
}
