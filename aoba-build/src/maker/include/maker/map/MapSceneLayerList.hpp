#pragma once

#include <QObject>
#include <QVector>

class MapScene;
class MapSceneLayer;

class MapSceneLayerList : public QObject {
  Q_OBJECT

public:
  //! Constructor
  MapSceneLayerList(QObject *parent) : QObject(parent) {}

  //! Deconstructor
  ~MapSceneLayerList();

  //! Removes all layers
  void clear();

  //! Adds a layer
  void addManaged(MapSceneLayer *);

  //! Returns all layers
  inline const QVector<MapSceneLayer*> &all() const { return mLayers; }

  //! Creates map layers
  void createLayersFromMap(MapScene *scene);

signals:
  //! Visibility changed
  void visiblityChanged();

private:
  //! Creates map layers
  void createTerrainLayersFromMap(MapScene *scene, const QString &switchId);
  
  //! Creates map layers
  void createTileLayersFromMap(MapScene *scene, const QString &switchId);
  
  //! Creates map layers
  void createCollisionLayersFromMap(MapScene *scene);
  
  //! Creates map layers
  void createComponentLayersFromMap(MapScene *scene);

  //! List of all layers
  QVector<MapSceneLayer*> mLayers;
};
