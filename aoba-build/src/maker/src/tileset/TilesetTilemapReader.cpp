#include <QPoint>
#include <QRect>
#include <QSize>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/tileset/TilesetTileRef.hpp>
#include <maker/tileset/TilesetTilemapReader.hpp>

// -----------------------------------------------------------------------------
void TilesetTilemapReader::setLayer(Aoba::TilesetLayer *layer) {
  mLayer = layer;

  emit layersChanged();
  emit sizeChanged();
  emit tilesChanged(QRect(QPoint(0, 0), size()));
}

// -----------------------------------------------------------------------------
QSize TilesetTilemapReader::size() const {
  if (!mLayer) {
    return QSize();
  }

  int numTiles = mLayer->allTiles().count();
  int numTilesX = 128 / mLayer->tileset()->assetSet()->tileSize().width();
  int numTilesY = (numTiles + numTilesX - 1) / numTilesX;

  return QSize(numTilesX, numTilesY);
}

// -----------------------------------------------------------------------------
QVector<Aoba::TilesetLayer*> TilesetTilemapReader::layers() const {
  if (!mLayer) {
    return {};
  }

  return {mLayer};
}

// -----------------------------------------------------------------------------
Aoba::TilesetTileRef TilesetTilemapReader::tileAt(Aoba::TilesetLayer *layer, const QPoint &pos) const {
  if (!mLayer || mLayer != layer) {
    return Aoba::TilesetTileRef();
  }

  QSize maxSize(size());
  if (pos.x() < 0 || pos.x() >= maxSize.width() || pos.y() < 0) {
    return Aoba::TilesetTileRef();
  }

  int index = pos.x() + pos.y() * maxSize.width();
  if (index >= mLayer->allTiles().count()) {
    return Aoba::TilesetTileRef();
  }

  return Aoba::TilesetTileRef(index, index);
}
