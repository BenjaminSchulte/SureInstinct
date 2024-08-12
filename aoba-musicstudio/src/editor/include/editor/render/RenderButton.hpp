#pragma once

#include <QString>
#include "RenderTheme.hpp"
#include "Renderable.hpp"

namespace Editor {

class RenderButton : public Renderable {
  Q_OBJECT

public:
  //! Constructor
  RenderButton(RenderWidget *parent, RenderTheme::IconId icon, const QRect &pos)
    : Renderable(parent, pos)
    , mIcon(icon)
  {}

  //! Draws this renderable
  void draw(RenderHelper &) override;

  //! The mouse move event
  bool mousePressEvent(Qt::MouseButton, const QPoint &) override;
  
  //! The mouse move event
  bool mouseEnterEvent() override;

  //! The mouse move event
  bool mouseLeaveEvent() override;

signals:
  //! Clicked
  void clicked();

protected:
  //! Whether the button is hovered
  bool mHovered = false;

  //! The icon name
  RenderTheme::IconId mIcon;
};

}