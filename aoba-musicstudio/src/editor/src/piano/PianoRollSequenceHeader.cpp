#include <QDebug>
#include <QPainter>
#include <editor/application/DisplayConfiguration.hpp>
#include <editor/piano/PianoRollSequenceHeader.hpp>
#include <editor/render/RenderHelper.hpp>
#include <sft/Configuration.hpp>
#include <sft/song/Song.hpp>

using namespace Editor;

// -----------------------------------------------------------------------------
void PianoRollSequenceHeader::draw(RenderHelper &h) {
  int mtw = h.config()->measureTickWidth();
  int leftTick = mScrollLeft / mtw;
  int sx = mScrollLeft % mtw;

  // MAIN AREA
  h.listItemBackground(QRect(mPosition.left() + mPianoWidth, mPosition.top(), mPosition.width() - mPianoWidth, mPosition.height()), h.color(QPalette::Background));
  drawSequences(h, leftTick, sx);
  
  // LEFT AREA
  h.listItemBackground(QRect(mPosition.left(), mPosition.top(), mPianoWidth, mPosition.height()), h.color(QPalette::Background));
}

// -----------------------------------------------------------------------------
void PianoRollSequenceHeader::drawSequences(RenderHelper &h, int leftTick, int sx) {
  if (!mChannel) {
    return;
  }

  (void)h;
  (void)leftTick;
  (void)sx;
/*
  auto info = mChannel->sequenceAtTick(leftTick);
  if (!info.isValid()) {
    return;
  }

  int mtw = h.config()->measureTickWidth();
  int headerLeft = (info.offset() - leftTick) * mtw;
  int index = info.index();
  while (headerLeft < mPosition.right() && index < mChannel->numSequences()) {
    Sft::SequencePtr sequence = mChannel->at(index);
    int headerWidth = sequence->duration() * mtw;
    drawSequence(h, sequence, headerLeft + mPianoWidth - sx, headerWidth);
    headerLeft += headerWidth;
    index++;
  }*/
}

// -----------------------------------------------------------------------------
//void PianoRollSequenceHeader::drawSequence(RenderHelper &h, const Sft::SequencePtr &sequence, int x, int width) {
//  h.listItemBackground(QRect(x, mPosition.top(), width, mPosition.height()), h.color(sequence->color()));
//}
