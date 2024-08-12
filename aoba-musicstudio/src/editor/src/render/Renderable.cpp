#include <QApplication>
#include <editor/render/Renderable.hpp>
#include <editor/render/RenderWidget.hpp>

using namespace Editor;

// -----------------------------------------------------------------------------
bool Renderable::mouseMoveEvent(const QPoint &localPoint) {
  QPoint point = localPoint + mPosition.topLeft();
  Renderable *renderable = renderableAtPosition(point);

  if (renderable != mCurrentHoverRenderable) {
    if (mCurrentHoverRenderable) {
      mCurrentHoverRenderable->mouseLeaveEvent();
    }
    mCurrentHoverRenderable = renderable;
    if (mCurrentHoverRenderable) {
      mCurrentHoverRenderable->mouseEnterEvent();
    }
  }

  if (mCurrentHoverRenderable) {
    mCurrentHoverRenderable->mouseMoveEvent(point - mCurrentHoverRenderable->position().topLeft());
    return true;
  } else {
    return false;
  }
}

// -----------------------------------------------------------------------------
bool Renderable::mousePressEvent(Qt::MouseButton button, const QPoint &localPoint) {
  QPoint point = localPoint + mPosition.topLeft();
  Renderable *renderable = renderableAtPosition(point);
  if (!renderable) {
    return false;
  }

  return renderable->mousePressEvent(button, point - renderable->position().topLeft());
}

// -----------------------------------------------------------------------------
bool Renderable::mouseReleaseEvent(Qt::MouseButton button, const QPoint &localPoint) {
  QPoint point = localPoint + mPosition.topLeft();
  Renderable *renderable = renderableAtPosition(point);
  if (!renderable) {
    return false;
  }

  return renderable->mouseReleaseEvent(button, point - renderable->position().topLeft());
}

// -----------------------------------------------------------------------------
bool Renderable::mouseEnterEvent() {
  return false;
}

// -----------------------------------------------------------------------------
bool Renderable::mouseDoubleClickEvent(Qt::MouseButton, const QPoint &) {
  return false;
}

// -----------------------------------------------------------------------------
bool Renderable::mouseLeaveEvent() {
  if (mCurrentHoverRenderable) {
    mCurrentHoverRenderable->mouseLeaveEvent();
    mCurrentHoverRenderable = nullptr;
  }

  return false;
}

// -----------------------------------------------------------------------------
bool Renderable::wheelEvent(QWheelEvent *event) {
  return zoomEvent(event->angleDelta().y() / 120.0);
}

// -----------------------------------------------------------------------------
void Renderable::drawAllRenderables(RenderHelper &helper) {
  for (Renderable *renderable : mRenderables) {
    renderable->draw(helper);
  }
}

// -----------------------------------------------------------------------------
Renderable *Renderable::renderableAtPosition(const QPoint &point) const {
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
DisplayConfiguration *Renderable::config() {
  return mParent->displayConfiguration();
}