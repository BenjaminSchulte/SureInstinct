#include <QMouseEvent>
#include <aoba/log/Log.hpp>
#include <maker/common/AobaGraphicsView.hpp>

// -----------------------------------------------------------------------------
void AobaGraphicsView::mousePressEvent(QMouseEvent *event) {
  if (event->button() != Qt::MiddleButton) {
    QGraphicsView::mousePressEvent(event);
    return;
  }

  setDragMode(QGraphicsView::DragMode::ScrollHandDrag);

  QMouseEvent* pressEvent = new QMouseEvent(
    QEvent::GraphicsSceneMousePress, 
    event->pos(),
    Qt::MouseButton::LeftButton,
    Qt::MouseButton::LeftButton,
    Qt::KeyboardModifier::NoModifier
  );

  setInteractive(false);
  QGraphicsView::mousePressEvent(pressEvent);
}

// -----------------------------------------------------------------------------
void AobaGraphicsView::mouseReleaseEvent(QMouseEvent *event) {
  if (event->button() != Qt::MiddleButton) {
    QGraphicsView::mouseReleaseEvent(event);
    return;
  }

  setDragMode(QGraphicsView::DragMode::NoDrag);
  setInteractive(true);
}
