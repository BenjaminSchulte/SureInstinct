#pragma once

#include <QPoint>
#include "RenderWidget.hpp"

namespace Editor {
class RenderScrollableWidget : public RenderWidget {
  Q_OBJECT

public:
  //! Constructor
  RenderScrollableWidget(QWidget *parent = nullptr)
    : RenderWidget(parent)
    , mCurrentScrollButton(Qt::NoButton)
  {}

  //! Updates the scrolling
  void updateScrolling();

  //! Returns the total scrollable widget size
  virtual QSize scrollableSize() const = 0;

  //! Scrolls the content
  void scroll(int x, int y);

  //! Scrolls the content
  virtual void doScroll(int, int);

protected:
  //! The mouse move event
  void mouseMoveEvent(QMouseEvent *e) override;

  //! The mouse move event
  void mousePressEvent(QMouseEvent *e) override;

  //! The mouse move event
  void mouseReleaseEvent(QMouseEvent *e) override;

  //! The current scroll position
  QPoint mScrollPosition;

  //! The current scroll mode
  Qt::MouseButton mCurrentScrollButton;

  //! The last mouse position
  QPoint mLastScrollMousePosition;
};

}