#pragma once

#include "../image/ImageCollection.hpp"

namespace Aoba {
class Tileset;
class TilesetTilePart;
class TilesetAnimationGroup;
class TilesetAssetSetTiles;
class TilesetAssetSetLayer;

class TilesetBuilder {
public:
  //! Constructor
  TilesetBuilder(Tileset *tileset) : mTileset(tileset) {}

  //! Loads the tileset
  bool build();

private:
  // Builds tiles
  bool buildTiles(TilesetAssetSetTiles *tiles);

  // Builds tiles
  bool buildTileImage(ImageCollection &collection, TilesetAssetSetTiles *tiles);

  // Builds tiles
  bool buildAnimatedTileData(ImageCollection &collection, TilesetAssetSetLayer *layer);

  // Builds tiles
  bool buildTileData(ImageCollection &collection, TilesetAssetSetTiles *tiles, TilesetAssetSetLayer *layer);

  //! Returns an hash for an animated tile
  QString hash(const TilesetTilePart &part, int partIndex) const;

  //! Returns an hash for an animated tile
  QString hash(const QString &id, int row, int partIndex) const;

  //! The tileset
  Tileset *mTileset;

  //! Map of animated tiles to their image collection
  QMap<QString, ImageCollectionPointer> mAnimatedTiles;

  //! List of all animation IDs
  QMap<QString, TilesetBuilderAnimation> mAnimations;
};

}