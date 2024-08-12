#pragma once

#include <QSharedPointer>
#include "../render/Renderable.hpp"

class QPainter;

namespace Sft {
class Channel;
typedef QSharedPointer<class Sequence> SequencePtr;
}

namespace Editor {

class PianoRollSequenceHeader : public Renderable {
public:
  //! Constructor
  PianoRollSequenceHeader(RenderWidget *parent, const QRect &pos) : Renderable(parent, pos) {}

  //! Sets the current channel
  inline void setChannel(Sft::Channel *channel) { mChannel = channel; };

  //! Sets the margin left
  inline void setScroll(int pw, int x) { mPianoWidth = pw; mScrollLeft = x; }

  //! Draws this renderable
  void draw(RenderHelper &) override;

protected:
  //! Draws all sequences
  void drawSequences(RenderHelper &h, int leftTick, int sx);

  //! Draws the sequence
  //void drawSequence(RenderHelper &h, const Sft::SequencePtr &sequence, int x, int width);

  //! The channel
  Sft::Channel *mChannel = nullptr;

  //! The piano width
  int mPianoWidth = 0;

  //! The left scroll
  int mScrollLeft = 0;
};

}