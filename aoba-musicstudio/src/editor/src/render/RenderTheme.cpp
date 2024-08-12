#include <QFont>
#include <editor/render/RenderTheme.hpp>

using namespace Editor;

// -----------------------------------------------------------------------------
RenderTheme::RenderTheme() {
  mPalette.setColor(QPalette::Window, QColor(164,164,192));
  mPalette.setColor(QPalette::WindowText, Qt::black);
  mPalette.setColor(QPalette::Base, QColor(53,53,64));
  mPalette.setColor(QPalette::AlternateBase, QColor(53,53,64).lighter(120));
  mPalette.setColor(QPalette::ToolTipBase, Qt::white);
  mPalette.setColor(QPalette::ToolTipText, Qt::white);
  mPalette.setColor(QPalette::Text, Qt::white);
  mPalette.setColor(QPalette::Button, QColor(164,164,192));
  mPalette.setColor(QPalette::ButtonText, Qt::black);
  mPalette.setColor(QPalette::BrightText, Qt::red);
  mPalette.setColor(QPalette::Link, QColor(200, 32, 110));
  mPalette.setColor(QPalette::Highlight, QColor(200, 32, 110));
  mPalette.setColor(QPalette::HighlightedText, Qt::white);
}

// -----------------------------------------------------------------------------
QFont RenderTheme::font(FontId id) const {
  switch (id) {
    case FontBold: return QFont("Arial", defaultFontSize(), QFont::Bold);
    case FontDefault: return QFont("Arial", defaultFontSize(), QFont::Normal);

    default: return QFont("Arial", defaultFontSize(), 100);
  }
  return QFont();
}

// -----------------------------------------------------------------------------
QImage RenderTheme::icon(IconId id, int size) const {
  (void)size;

  switch (id) {
    case IconAdd: return QImage(":/images/16x16/add.png");
    case IconSpeaker: return QImage(":/images/16x16/speaker.png");

    default: return QImage();
  }
}