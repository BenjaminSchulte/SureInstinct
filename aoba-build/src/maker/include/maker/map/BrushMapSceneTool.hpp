#pragma once

#include "MapSceneTool.hpp"

class QGraphicsRectItem;

class BrushMapSceneTool : public MapSceneTool {
  Q_OBJECT

public:
  //! Mouse event
  bool onMouseDown(const QPoint &, QGraphicsSceneMouseEvent *) override;
  
  //! Mouse event
  bool onMouseUp(const QPoint &, QGraphicsSceneMouseEvent *) override;
  
  //! Mouse event
  bool onMouseMove(const QPoint &, QGraphicsSceneMouseEvent *) override;

  //! Mouse event
  bool onMouseLeave() override;

protected:
  //! Plots the object
  void plotAt(const QPoint &);

  //! Plots the object
  void commit();

  //! Clears all items
  void clear() override;

  //! Whether the mouse is currently pressed
  bool mLeftButtonDown = false;

  //! The start position
  QPoint mStartPosition;

  //! The selection rect
  QGraphicsRectItem *mSelectionRect = nullptr;
};