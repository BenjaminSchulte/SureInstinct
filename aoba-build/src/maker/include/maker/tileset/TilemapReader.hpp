#pragma once

#include <QObject>
#include <QVector>

namespace Rpg {
struct TilesetTileCollision;
}

namespace Aoba {
class TilesetLayer;
struct TilesetTileRef;
}

class TilemapReader : public QObject {
  Q_OBJECT

public:
  //! Constructor
  TilemapReader(QObject *parent = nullptr) : QObject(parent) {}

  //! Deconstructor
  virtual ~TilemapReader() = default;

  //! Returns the size
  virtual QSize size() const = 0;

  //! Returns the tileset layer
  virtual QVector<Aoba::TilesetLayer*> layers() const = 0;

  //! Returns the tile at a position
  virtual Aoba::TilesetTileRef tileAt(Aoba::TilesetLayer*, const QPoint &) const = 0;

signals:
  //! The layer selection has changed
  void layersChanged();

  //! The size changed
  void sizeChanged();

  //! The tiles changed
  void tilesChanged(const QRect &);
};