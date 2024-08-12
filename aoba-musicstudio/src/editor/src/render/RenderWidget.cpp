#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <editor/application/DisplayConfiguration.hpp>
#include <editor/render/Renderable.hpp>
#include <editor/render/RenderTheme.hpp>
#include <editor/render/RenderWidget.hpp>

using namespace Editor;

// -----------------------------------------------------------------------------
RenderWidget::RenderWidget(QWidget *parent)
  : QOpenGLWidget(parent)
  , mConfiguration(nullptr)
  , mCurrentHoverRenderable(nullptr)
{
  setMouseTracking(true);
  setMinimumSize(QSize(200, 200));
}

// -----------------------------------------------------------------------------
RenderWidget::~RenderWidget() {
  removeAllRenderables();
}

// -----------------------------------------------------------------------------
RenderTheme *RenderWidget::theme() const {
  return mConfiguration->theme();
}

// -----------------------------------------------------------------------------
QColor RenderWidget::color(QPalette::ColorRole role) const {
  if (!isRenderable()) {
    return QColor();
  }

  return theme()->palette().color(role);
}

// -----------------------------------------------------------------------------
QImage RenderWidget::icon(RenderTheme::IconId icon, int size) const {
  if (!isRenderable()) {
    return QImage();
  }

  return theme()->icon(icon, size);
}

// -----------------------------------------------------------------------------
void RenderWidget::removeAllRenderables() {
  for (Renderable *renderable : mRenderables) {
    delete renderable;
  }

  mRenderables.clear();
  mCurrentHoverRenderable = nullptr;
}

// -----------------------------------------------------------------------------
void RenderWidget::drawAllRenderables(RenderHelper &helper) {
  for (Renderable *renderable : mRenderables) {
    renderable->draw(helper);
  }
}

// -----------------------------------------------------------------------------
Renderable *RenderWidget::renderableAtPosition(const QPoint &point) const {
  QVectorIterator<Renderable*> it(mRenderables);
  it.toBack();
  while (it.hasPrevious()) {
    Renderable *renderable = it.previous();
    if (renderable->position().contains(point)) {
      return renderable;
    }
  }
  return nullptr;
}

// -----------------------------------------------------------------------------
void RenderWidget::mouseMoveEvent(QMouseEvent *e) {
  if (!isRenderable()) { return; }

  Renderable *renderable = renderableAtPosition(e->pos());

  if (!mMouseIsPressed && renderable != mCurrentHoverRenderable) {
    if (mCurrentHoverRenderable) {
      mCurrentHoverRenderable->mouseLeaveEvent();
    }
    mCurrentHoverRenderable = renderable;
    if (mCurrentHoverRenderable) {
      mCurrentHoverRenderable->mouseEnterEvent();
    }
  }

  if (mCurrentHoverRenderable) {
    mCurrentHoverRenderable->mouseMoveEvent(e->pos() - mCurrentHoverRenderable->position().topLeft());
  }
}

// -----------------------------------------------------------------------------
void RenderWidget::mouseDoubleClickEvent(QMouseEvent *e) {
  if (!isRenderable()) { return; }

  Renderable *renderable = renderableAtPosition(e->pos());
  if (renderable) {
    renderable->mouseDoubleClickEvent(e->button(), e->pos() - renderable->position().topLeft());
  }
}

// -----------------------------------------------------------------------------
void RenderWidget::wheelEvent(QWheelEvent *e) {
  if (!isRenderable()) { return; }

  if (mCurrentHoverRenderable) {
    mCurrentHoverRenderable->wheelEvent(e);
  }
}

// -----------------------------------------------------------------------------
void RenderWidget::leaveEvent(QEvent *) {
  if (mCurrentHoverRenderable) {
    mCurrentHoverRenderable->mouseLeaveEvent();
    mCurrentHoverRenderable = nullptr;
  }
}

// -----------------------------------------------------------------------------
void RenderWidget::mousePressEvent(QMouseEvent *e) {
  if (!isRenderable()) { return; }

  Renderable *renderable = renderableAtPosition(e->pos());
  if (!renderable) {
    return;
  }

  mMouseIsPressed = true;
  renderable->mousePressEvent(e->button(), e->pos() - renderable->position().topLeft());
}

// -----------------------------------------------------------------------------
void RenderWidget::mouseReleaseEvent(QMouseEvent *e) {
  if (!isRenderable()) { return; }

  mMouseIsPressed = false;
  if (mCurrentHoverRenderable) {
    mCurrentHoverRenderable->mouseReleaseEvent(e->button(), e->pos() - mCurrentHoverRenderable->position().topLeft());
  }
}

// -----------------------------------------------------------------------------
bool RenderWidget::isRenderable() const {
  return !!mConfiguration;
}

// -----------------------------------------------------------------------------
void RenderWidget::initializeGL() {
}

// -----------------------------------------------------------------------------
void RenderWidget::resizeGL(int, int) {
  if (mConfiguration) {
    recreateContent(); 
  }
}

// -----------------------------------------------------------------------------
void RenderWidget::paintGL() {
  if (!isRenderable()) { return; }

  QPainter painter;
  painter.begin(this);
  painter.setRenderHint(QPainter::Antialiasing);
  
  QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
  drawGL(&painter, f);

  painter.end();
}