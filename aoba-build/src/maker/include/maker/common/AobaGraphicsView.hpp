#pragma once

#include <QGraphicsView>

class AobaGraphicsView : public QGraphicsView {
public:
  //! Constructor
  AobaGraphicsView(QWidget *parent = nullptr) : QGraphicsView(parent) {}

  //! Whether the view is dragging
  inline bool isDragging() const { return dragMode() == QGraphicsView::DragMode::ScrollHandDrag; }

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
};
