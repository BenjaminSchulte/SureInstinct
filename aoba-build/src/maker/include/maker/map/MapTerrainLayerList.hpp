#pragma once

#include <QListWidget>
#include <QPixmap>
#include <QVector>

namespace Aoba {
class TilesetTerrainLayer;
class TilesetTerrainType;
}

class MapTerrainLayerList : public QListWidget {
  Q_OBJECT

public:
  //! Constructor
  MapTerrainLayerList(QWidget *parent);

  //! Sets the terrain layer
  void setLayer(Aoba::TilesetTerrainLayer *layer);

  //! Returns the current layer
  inline Aoba::TilesetTerrainLayer *layer() const { return mLayer; }

  //! Creates the terrain list
  void recreateTerrainList();

  //! Returns the current brush
  int brush() const;

signals:
  //! Updates the terrain
  void terrainChanged(int);

private slots:
  //! Updates the brush
  void setBrushFromCurrentRow();

private:
  //! The layer
  Aoba::TilesetTerrainLayer *mLayer;

  //! The empty pixmap
  QPixmap mEmptyPixmap;
};
