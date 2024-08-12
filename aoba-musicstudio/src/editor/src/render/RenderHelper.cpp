#include <QPainter>
#include <QLinearGradient>
#include <sft/types/Color.hpp>
#include <editor/render/RenderHelper.hpp>
#include <editor/render/RenderTheme.hpp>
#include <editor/application/DisplayConfiguration.hpp>

using namespace Editor;

// -----------------------------------------------------------------------------
void RenderHelper::clear() {
  mPainter->fillRect(QRect(0, 0, mSize.width(), mSize.height()), color(QPalette::Base));
}

// -----------------------------------------------------------------------------
RenderTheme *RenderHelper::theme() const {
  return mConfig->theme();
}

// -----------------------------------------------------------------------------
QImage RenderHelper::icon(RenderTheme::IconId id, int size) const {
  return theme()->icon(id, size);
}

// -----------------------------------------------------------------------------
QColor RenderHelper::color(QPalette::ColorRole role) const {
  return theme()->palette().color(role);
}

// -----------------------------------------------------------------------------
QColor RenderHelper::color(const Sft::Color &c) const {
  return RenderHelper::toQColor(c);
}

// -----------------------------------------------------------------------------
QColor RenderHelper::toQColor(const Sft::Color &c) {
  return QColor(c.red(), c.green(), c.blue(), c.alpha());
}

// -----------------------------------------------------------------------------
QFont RenderHelper::font(RenderTheme::FontId id) const {
  return theme()->font(id);
}

// -----------------------------------------------------------------------------
void RenderHelper::text(const QRect &rect, const QString &text, const QColor &color, RenderTheme::FontId fontId, int alignFlags) {
  mPainter->setFont(font(fontId));
  mPainter->setPen(QPen(color));
  mPainter->drawText(rect, alignFlags, text);
}

// -----------------------------------------------------------------------------
void RenderHelper::header(const QRect &rect, const QString &headerText) {
  listItemBackground(rect, color(QPalette::Window));
  text(rect, headerText, color(QPalette::WindowText), RenderTheme::FontBold, Qt::AlignCenter);
}

// -----------------------------------------------------------------------------
void RenderHelper::listItemBackground(const QRect &rect, const QColor &color, float opacity) {
  QLinearGradient gradient(0, rect.top(), 0, rect.bottom());
  QGradientStops stops({
    QGradientStop(0, color.lighter(120)),
    QGradientStop(0.5, color)
  });
  gradient.setStops(stops);
  mPainter->setOpacity(opacity);
  mPainter->fillRect(rect, gradient);
  mPainter->setPen(QPen(Qt::black));
  mPainter->drawRect(rect);
  mPainter->setOpacity(1.0f);
}