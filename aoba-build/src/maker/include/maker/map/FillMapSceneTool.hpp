#pragma once

#include "MapSceneTool.hpp"

class FillMapSceneTool : public MapSceneTool {
  Q_OBJECT

public:
  //! Mouse event
  bool onMouseDown(const QPoint &, QGraphicsSceneMouseEvent *) override;
  
  //! Mouse event
  bool onMouseMove(const QPoint &, QGraphicsSceneMouseEvent *) override;

  //! Mouse event
  bool onMouseLeave() override;

protected:
  //! Plots the object
  void fillAt(const QPoint &);

  //! Clears all items
  void clear() override;

  //! The selection rect
  QGraphicsRectItem *mSelectionRect = nullptr;
};