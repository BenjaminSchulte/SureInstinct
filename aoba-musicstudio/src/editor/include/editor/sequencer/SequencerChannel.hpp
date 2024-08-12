#pragma once

#include "../render/Renderable.hpp"

namespace Sft {
struct MmlExecutedCommand;
}

namespace Editor {
class ProjectContext;

class SequencerChannel : public Renderable {
public:
  //! Constructor
  SequencerChannel(RenderWidget *parent, const QRect &rect, ProjectContext *song, int channel);

  //! Draws this renderable
  void draw(RenderHelper &) override;

  //! The mouse move event
  bool mousePressEvent(Qt::MouseButton, const QPoint &) override;

  //! The mouse move event
  bool mouseMoveEvent(const QPoint &) override;

  //! The mouse move event
  bool mouseReleaseEvent(Qt::MouseButton, const QPoint &) override;

  //! Sets the scroll
  void setScroll(int sx) { mScrollX = sx; }

  //! Sets the zoom
  inline void setZoom(double zoom) { mZoom = zoom; }

protected:
  //! Draws this renderable
  void drawNotes(RenderHelper &) const;

  //! Draws this renderable
  void drawNote(RenderHelper &, const Sft::MmlExecutedCommand &, int offset) const;

  //! The channel
  ProjectContext *mContext;

  //! The channel
  int mChannel;

  //! The scroll X
  int mScrollX = 0;

  //! The zoom
  double mZoom = 0.5;

  //! Whether we are setting the editor position currently
  bool mMouseSetsEditorPosition = false;
};

}