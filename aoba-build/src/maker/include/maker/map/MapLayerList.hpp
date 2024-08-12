#pragma once

#include <QSignalMapper>
#include <QTableWidget>
#include <QVector>

namespace Aoba {
class TilemapAssetSetLayer;
}

class QCheckBox;
class MapSceneLayerList;
class MapSceneLayer;

class MapLayerList : public QTableWidget {
  Q_OBJECT

public:
  //! Constructor
  MapLayerList(QWidget *parent = nullptr);

  //! Selects a layer
  void selectTilemapLayer(Aoba::TilemapAssetSetLayer *);

signals:
  void layerChanged(MapSceneLayer*);

public slots:
  //! Sets the current layer list
  void setLayerList(MapSceneLayerList*);

  //! Sets the selected layer
  void setSelectedLayer(MapSceneLayer*);

  //! The selected row changed
  void selectedRowChanged();

private slots:
  //! The visibility has been changed
  void visibilityChanged(int id);

private:
  //! Removes all layers
  void removeLayers();

  //! Adds a layer
  void addLayer(MapSceneLayer*);

  //! Returns the row from ID
  int rowFromId(int id) const;

  //! Returns the layer of a row
  MapSceneLayer *layerFromRow(int id) const;

  //! List of all connected layers
  QVector<MapSceneLayer*> mLayers;

  //! Maps a id to a checkbox
  QMap<int, QCheckBox*> mVisibleCheckBoxes;

  //! Visible mapper for checkbox
  QSignalMapper mVisibleMapper;
};
