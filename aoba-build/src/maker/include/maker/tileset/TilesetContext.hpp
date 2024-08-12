#pragma once

#include <QObject>
#include <QBrush>
#include <QMap>
#include <QPixmap>
#include "TileBrush.hpp"
#include "TilesetTilemapReader.hpp"

namespace Aoba {
class Tileset;
class TilesetLayer;
}

namespace Maker {
class GameTilesetAsset;
}

class TilesetContext : public QObject {
  Q_OBJECT

public:
  //! Constructor
  explicit TilesetContext(QObject *parent = nullptr);

  //! Deconstructor
  virtual ~TilesetContext() = default;

  //! Loads a tileset
  virtual void setTileset(Aoba::Tileset *set);

  //! Loads a tileset layer
  void setTilesetLayer(Aoba::TilesetLayer *layer);

  //! Sets the tile brush
  void setTileBrush(const TileBrush &brush);

  //! Returns the tileset
  inline Aoba::Tileset *tileset() const { return mTileset; }

  //! Returns the tileset
  inline Aoba::TilesetLayer *tilesetLayer() const { return mTilesetLayer; }

  //! Tile brush
  inline const TileBrush &tileBrush() const { return mTileBrush; }

  //! Returns the tileset reader
  inline TilemapReader *tilesetReader() { return &mTilesetReader; }

  //! Sets the brush flip
  void setBrushFlip(bool x, bool y);

  //! Returns the brush flip X
  inline bool brushFlipX() const { return mBrushFlipX; }

  //! Returns the brush flip Y
  inline bool brushFlipY() const { return mBrushFlipY; }

signals:
  //! The tileset has been changed
  void tilesetChanged();

  //! The tileset layer has been changed
  void tilesetLayerChanged();

  //! The tile brush changed
  void brushChanged();

  //! Redraw requested
  void redrawRequested();
  
protected:
  //! Flips the brush
  void flipBrush(bool x, bool y);

  //! The tileset
  Aoba::Tileset *mTileset = nullptr;

  //! The tileset layer
  Aoba::TilesetLayer *mTilesetLayer = nullptr;

  //! The current brush
  TileBrush mTileBrush;

  //! The current terrain brush
  TileBrush mTerrainBrush;

  //! The brush flip
  bool mBrushFlipX = false;

  //! The brush flip
  bool mBrushFlipY = false;

  //! The tileset reader
  TilesetTilemapReader mTilesetReader;
};