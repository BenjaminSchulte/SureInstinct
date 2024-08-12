#pragma once 

#include <QObject>
#include <QPainter>
#include <QPixmap>
#include <QGraphicsView>
#include "../tileset/TileBrush.hpp"
#include "MapEditorContext.hpp"

namespace Aoba {
class TilemapAssetSetLayer;
class TilemapTerrainLayer;
}

class MapScene;

struct MapSceneLayerRenderInfo {
  //! Constructor
  MapSceneLayerRenderInfo(MapScene *scene, const QRect &tileRect);

  //! Returns the map scene
  inline MapScene *mapScene() const { return mScene; }

  //! Returns the render rect
  inline const QRect &tileRect() const { return mTileRect; }

  //! Returns the render rect
  inline const QRect &renderRect() const { return mRenderRect; }

  //! Returns the pixmap
  inline QPixmap &mainPixmap() { return mMainPixmap; }

  //! Returns the pixmap
  inline QPixmap &subPixmap() { return mSubPixmap; }

  //! Returns the pixmap
  inline QPixmap &overlayPixmap() { return mOverlayPixmap; }

private:
  MapScene *mScene;
  QRect mTileRect;
  QRect mRenderRect;
  QPixmap mMainPixmap;
  QPixmap mSubPixmap;
  QPixmap mOverlayPixmap;
};

class MapSceneLayer : public QObject {
  Q_OBJECT

public:
  //! Constructor
  MapSceneLayer(const QString &name);

  //! Deconstructor
  virtual ~MapSceneLayer() {
    clearSceneItems();
  }

  //! Returns an unique ID
  inline int id() const { return mId; }

  //! Returns the layer name
  const QString &name() const { return mName; }

  //! Returns whether this layer is visible
  inline bool isVisible() const { return mVisible; }

  //! Returns whether the layer is active or not
  inline bool isActive() const { return mActive; }

  //! Sets the visibility
  void setVisible(bool visible);

  //! Sets the layer active
  virtual void setActive(bool active) { mActive = active; }

  //! Renders the screen
  virtual void renderScreen(MapSceneLayerRenderInfo &) {}

  //! Renders the screen
  virtual void renderOverlay(MapSceneLayerRenderInfo &, QPainter &) {}

  //! Returns the layer if provided
  virtual Aoba::TilemapAssetSetLayer *layer() const { return nullptr; } 

  //! Returns the layer if provided
  virtual QString terrainLayerId() const { return ""; }

  //! Returns whether this is an NPC layer
  virtual bool isObjectLayer() const { return false; }

  //! Returns the matching brush
  virtual const TileBrush &brush(const MapScene*) const { return mInvalidBrush; }

  //! Returns a tile
  virtual TileBrushTile getTile(const MapScene*, const QPoint &) const { return TileBrushTile(); }

  //! Sets a tile
  virtual void setTiles(MapScene*, const PositionedTileList &) const {}

  //! Commits changed
  virtual void commitSetTile(MapScene*) {}

  //! Clears all scene items
  void clearSceneItems();

  //! Whether the layer supports a tool
  virtual bool supportsTool(MapEditTool tool) const {
    switch (tool) {
      case MapEditTool::BRUSH: return true;
      case MapEditTool::FILL: return true;
      default: return false;
    }
  }

  //! Returns the default tool
  virtual MapEditTool fallbackTool() const { return MapEditTool::BRUSH; }

signals:
  //! Whether the visibility changed
  void visibleChanged();

protected:
  //! Adds a scene item
  void addSceneItem(MapSceneLayerRenderInfo &, QGraphicsItem*);

  //! The ID of this layer
  int mId;

  //! The layer name
  QString mName;

  //! Visible
  bool mVisible = true;

  //! Whether the layer is active
  bool mActive = false;

  //! List of all scene items
  QVector<QGraphicsItem*> mSceneItems;

  //! Temporary invalid brush
  static TileBrush mInvalidBrush;
};
