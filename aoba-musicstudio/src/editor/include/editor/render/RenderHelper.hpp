#pragma once

#include <QString>
#include <QPalette>
#include "RenderTheme.hpp"

class QPainter;
class QSize;

namespace Sft {
struct Color;
}

namespace Editor {
class DisplayConfiguration;

class RenderHelper {
public:
  //! Constructor
  RenderHelper(QPainter *painter, DisplayConfiguration *config, const QSize &size)
    : mPainter(painter), mConfig(config), mSize(size) {}

  //! Clears the background
  void clear();

  //! Draws a text
  void text(const QRect &rect, const QString &text, const QColor &color, RenderTheme::FontId font=RenderTheme::FontDefault, int alignFlags = 0);

  //! Draws a header
  void header(const QRect &rect, const QString &text);

  //! Draws a list item background
  void listItemBackground(const QRect &rect, const QColor &color, float opacity=1.0f);

  //! Returns the painter
  inline QPainter *painter() { return mPainter; }

  //! Returns the config
  inline DisplayConfiguration *config() { return mConfig; }
  
  //! Returns a font
  QImage icon(RenderTheme::IconId id, int size=16) const;

  //! Returns a font
  QFont font(RenderTheme::FontId id) const;

  //! Returns a color
  QColor color(QPalette::ColorRole) const;

  //! Returns a color
  inline QColor color(const QColor &color) const { return color; }

  //! Returns a color
  QColor color(const Sft::Color &) const;

  //! Returns a color
  static QColor toQColor(const Sft::Color &);
protected:

  //! Returns the theme
  RenderTheme *theme() const;

  //! The painter
  QPainter *mPainter;

  //! The configuration
  DisplayConfiguration *mConfig;

  //! The size
  QSize mSize;
};

}