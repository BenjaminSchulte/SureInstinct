#pragma once

#include "../render/Renderable.hpp"

namespace Editor {
class ProjectContext;

class PianoRollPiano : public Renderable {
public:
  //! Constructor
  PianoRollPiano(RenderWidget *parent, const QRect &pos) : Renderable(parent, pos) {}

  //! Sets the top scroll
  inline void setScroll(int y) { mScrollTop = y; }

  //! Sets the active instrument
  void setProject(ProjectContext *context) { mContext = context; }

  //! Draws this renderable
  void draw(RenderHelper &) override;

  //! The mouse move event
  bool mouseMoveEvent(const QPoint &) override;

  //! The mouse move event
  bool mousePressEvent(Qt::MouseButton, const QPoint &) override;

  //! The mouse move event
  bool mouseReleaseEvent(Qt::MouseButton, const QPoint &) override;

protected:
  //! The scroll top
  int mScrollTop = 0;

  //! Mouse is down
  bool mMouseIsDown = false;

  //! Current note to play
  int mLastNote = -1;

  //! Current instrument
  ProjectContext *mContext;
};

}