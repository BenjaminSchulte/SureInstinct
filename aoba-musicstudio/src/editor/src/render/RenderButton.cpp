#include <QPainter>
#include <editor/render/RenderHelper.hpp>
#include <editor/render/RenderButton.hpp>
#include <editor/render/RenderWidget.hpp>

using namespace Editor;

// --------------------------------------------------------------------
void RenderButton::draw(RenderHelper &h) {
  QPainter *p = h.painter();
  
  if (mHovered) {
    p->fillRect(mPosition, QColor(255, 255, 255, 128));
  }
  p->setPen(QPen(h.color(QPalette::WindowText)));
  p->drawRect(mPosition);

  QImage icon = h.icon(mIcon);
  p->drawImage(QPoint(mPosition.center().x() + 1 - icon.width() / 2, mPosition.center().y() + 1 - icon.height() / 2), icon);
}

// --------------------------------------------------------------------
bool RenderButton::mousePressEvent(Qt::MouseButton button, const QPoint &) {
  if (button == Qt::LeftButton) {
    emit clicked();
    return true;
  }

  return false;
}

// --------------------------------------------------------------------
bool RenderButton::mouseEnterEvent() {
  mHovered = true;
  mParent->update();
  return true;
}

// --------------------------------------------------------------------
bool RenderButton::mouseLeaveEvent() {
  mHovered = false;
  mParent->update();
  return true;
}
