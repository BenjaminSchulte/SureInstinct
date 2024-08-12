#include <QDebug>
#include <QMouseEvent>
#include <QApplication>
#include <editor/render/Renderable.hpp>
#include <editor/render/RenderScrollableWidget.hpp>

using namespace Editor;

// -----------------------------------------------------------------------------
void RenderScrollableWidget::updateScrolling() {
  QSize area(size());

  int maxX = qMax(0, scrollableSize().width() - area.width());
  int maxY = qMax(0, scrollableSize().height() - area.height());
  int scrollX = 0;
  int scrollY = 0;

  if (mScrollPosition.x() > maxX) {
    scrollX = maxX - mScrollPosition.x();
  }
  if (mScrollPosition.y() > maxY) {
    scrollY = maxY - mScrollPosition.y();
  }

  if (scrollX != 0 || scrollY != 0) {
    mScrollPosition += QPoint(scrollX, scrollY);
    scroll(scrollX, scrollY);
  }
}

// -----------------------------------------------------------------------------
void RenderScrollableWidget::scroll(int x, int y) {
  QSize area(size());
  int maxX = qMax(0, scrollableSize().width() - area.width());
  int maxY = qMax(0, scrollableSize().height() - area.height());

  if (mScrollPosition.x() + x > maxX) {
    x = maxX - mScrollPosition.x();
  }
  if (mScrollPosition.x() + x < 0) {
    x = 0 - mScrollPosition.x();
  }

  if (mScrollPosition.y() + y > maxY) {
    y = maxY - mScrollPosition.y();
  }
  if (mScrollPosition.y() + y < 0) {
    y = 0 - mScrollPosition.y();
  }

  if (x != 0 || y != 0) {
    mScrollPosition += QPoint(x, y);
    doScroll(x, y);
  }
}

// -----------------------------------------------------------------------------
void RenderScrollableWidget::doScroll(int x, int y) {
  for (auto *renderable : mRenderables) {
    QRect pos(renderable->position());
    pos.translate(QPoint(-x, -y));
    renderable->setPosition(pos);
  }

  update();
}

// -----------------------------------------------------------------------------
void RenderScrollableWidget::mouseMoveEvent(QMouseEvent *e) {
  if (mCurrentScrollButton != Qt::NoButton) {
    QPoint scrollPower(e->globalPos() - mLastScrollMousePosition);
    scroll(-scrollPower.x(), -scrollPower.y());
    mLastScrollMousePosition = e->globalPos();
  } else {
    RenderWidget::mouseMoveEvent(e);
  }
}

// -----------------------------------------------------------------------------
void RenderScrollableWidget::mousePressEvent(QMouseEvent *e) {
  if (e->button() == Qt::MidButton || (e->button() == Qt::LeftButton && QApplication::keyboardModifiers() & Qt::AltModifier)) {
    mCurrentScrollButton = e->button();
    mLastScrollMousePosition = e->globalPos();
  } else {
    RenderWidget::mousePressEvent(e);
  }
}

// -----------------------------------------------------------------------------
void RenderScrollableWidget::mouseReleaseEvent(QMouseEvent *e) {
  if (e->button() == mCurrentScrollButton) {
    mCurrentScrollButton = Qt::NoButton;
    
    QPoint scrollPower(e->globalPos() - mLastScrollMousePosition);
    scroll(-scrollPower.x(), -scrollPower.y());
    RenderWidget::mouseMoveEvent(e);
  } else {
    RenderWidget::mouseReleaseEvent(e);
  }
}