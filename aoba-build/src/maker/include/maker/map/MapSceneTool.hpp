#pragma once

#include <QObject>
#include <QPoint>
#include "MapScene.hpp"

class BaseTileScene;
class QGraphicsSceneMouseEvent;
class QGraphicsItem;

class MapSceneTool : public QObject {
  Q_OBJECT

public:
  //! Deconsturctor
  ~MapSceneTool() {
    clear();
  }

  //! Initialize
  virtual void initialize(BaseTileScene *scene) { mScene = scene; }

  //! Mouse event
  virtual bool onMouseDown(const QPoint &, QGraphicsSceneMouseEvent *) { return false; }

  //! Mouse event
  virtual bool onMouseUp(const QPoint &, QGraphicsSceneMouseEvent *) { return false; }

  //! Mouse event
  virtual bool onMouseMove(const QPoint &, QGraphicsSceneMouseEvent *) { return false; }

  //! Mouse event
  virtual bool onMouseMoveOuter(const QPoint &, QGraphicsSceneMouseEvent *) {
    return onMouseLeave();
  }

  //! Mouse event
  virtual bool onMouseLeave() { return false; }

protected:
  //! Clears all items
  virtual void clear();

  //! Adds an item
  void add(QGraphicsItem *);

  //! The scene
  BaseTileScene *mScene;

  //! All items
  QVector<QGraphicsItem*> mItems;
};