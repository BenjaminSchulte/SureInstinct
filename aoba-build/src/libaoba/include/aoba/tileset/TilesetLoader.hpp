#pragma once

#include <QImage>
#include <QSharedPointer>
#include <yaml-cpp/yaml.h>

typedef QSharedPointer<class QImage> QImagePtr;

namespace Aoba {
class CsvReader;
class Tileset;
class TilesetAssetSetTiles;

class TilesetLoader {
public:
  //! Constructor
  TilesetLoader(Tileset *tileset) : mTileset(tileset) {}

  //! Loads the tileset
  bool load();

  //! Loads tiles
  bool loadBgTiles(TilesetAssetSetTiles *tiles, const QString &imageFilePath, const QString &paletteFilePath);

  //! Loads tiles
  bool loadBgTiles(TilesetAssetSetTiles *tiles, const QImagePtr &tilesImage, const QImagePtr &paletteImage);

private:
  //! Loads the configuration
  bool loadConfiguration();

  //! Loads the configuration
  bool loadAnimations(const YAML::Node &node);

  //! Loads the configuration
  bool loadAnimation(const QString &id, const YAML::Node &node);

  //! Loads layer
  bool loadLayer(TilesetAssetSetLayer *layer);

  //! Creates a tile
  TilesetTile *createTile(TilesetLayer *layer, CsvReader &csv);

  //! The tileset
  Tileset *mTileset;
};

}