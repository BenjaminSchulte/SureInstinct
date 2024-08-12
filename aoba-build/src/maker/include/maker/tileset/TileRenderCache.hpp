#pragma once

#include <QMap>
#include <QVector>
#include <QPixmap>

namespace Aoba {
class Tileset;
class TilesetLayer;
}

struct TileRenderCacheLayer {
  QVector<QPixmap> frontTiles;
  QVector<QPixmap> backTiles;
};

class TileRenderCache {
public:
  //! Constructor
  TileRenderCache(int maxRecords=10) : mMaxRecords(maxRecords) {}

  //! Returns the cache for a record
  const TileRenderCacheLayer &get(Aoba::TilesetLayer *layer);

  //! Clears the cache
  inline void clear() {
    mUsageOrder.clear();
    mCache.clear();
  }

private:
  //! Renders a tileset
  void cacheLayer(Aoba::TilesetLayer *layer);

  //! Maximum amount of records
  int mMaxRecords;

  //! The usage counter
  QVector<Aoba::TilesetLayer*> mUsageOrder;

  //! Cache layer
  QMap<Aoba::TilesetLayer*, TileRenderCacheLayer> mCache;
};