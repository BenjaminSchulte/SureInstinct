#include <QByteArray>
#include <QVector>
#include <aoba/tileset/BgTile.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
QByteArray BgTile::toSnesBgTiles(const QVector<BgTile> &bgTiles) {
  uint32_t bytesPerTile = 2;

  QByteArray bgTileBuffer;
  bgTileBuffer.resize(bgTiles.count() * bytesPerTile);
  char *bgTileBufferPtr = bgTileBuffer.data();

  for (const auto &bgTile : bgTiles) {
    uint16_t data = bgTile.toSnesBgTile();
    bgTileBufferPtr[0] = data;
    bgTileBufferPtr[1] = data >> 8;
    bgTileBufferPtr += bytesPerTile;
  }

  return bgTileBuffer;
}