#pragma once

#include "TilemapReader.hpp"

class TilesetTilemapReader : public TilemapReader {
  Q_OBJECT

public:
  //! Constructor
  TilesetTilemapReader(Aoba::TilesetLayer *layer, QObject *parent = nullptr)
    : TilemapReader(parent)
    , mLayer(layer) {}
  
  //! Sets the layer
  void setLayer(Aoba::TilesetLayer *);

  //! Returns the size
  QSize size() const override;

  //! Returns the tileset layer
  QVector<Aoba::TilesetLayer*> layers() const override;

  //! Returns the tile at a position
  Aoba::TilesetTileRef tileAt(Aoba::TilesetLayer*, const QPoint &) const override;

private:
  //! The current tileset layer
  Aoba::TilesetLayer *mLayer;
};