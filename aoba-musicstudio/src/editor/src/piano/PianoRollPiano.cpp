#include <QDebug>
#include <QPainter>
#include <editor/application/DisplayConfiguration.hpp>
#include <editor/piano/PianoRollPiano.hpp>
#include <editor/render/RenderHelper.hpp>
#include <editor/project/ProjectContext.hpp>
#include <editor/playback/AudioPlayer.hpp>
#include <sft/channel/Channel.hpp>
#include <sft/Configuration.hpp>

using namespace Editor;

// -----------------------------------------------------------------------------
void PianoRollPiano::draw(RenderHelper &h) {
  int bottom = mPosition.bottom();
  int width = mPosition.width();
  int pkh = h.config()->pianoKeyHeight();

  int topNoteTotal = mScrollTop / pkh;
  int topOctave = topNoteTotal / Sft::NUM_NOTES_PER_OCTAVE + Sft::FIRST_OCTAVE;
  int topNote = topNoteTotal % Sft::NUM_NOTES_PER_OCTAVE;

  QRect full(0, 0, mPosition.right(), bottom);

  QPainter *painter = h.painter();
  int y = -(mScrollTop % pkh) + mPosition.top();

  bool first = true;

  while (y < bottom) {
    if (first || topNote == 0) {
      h.listItemBackground(QRect(0, y - topNote * pkh, width, pkh * Sft::NUM_NOTES_PER_OCTAVE), h.color(QPalette::Window));
      first = false;
    }

    // DRAW HALF NOTES
    switch (topNote) {
      case 7:
        painter->setPen(QPen(h.color(QPalette::Window).darker(140)));
        painter->drawLine(QPoint(0, y), QPoint(width - 2, y));
        break;

      case 1:
      case 3:
      case 5:
      case 8:
      case 10:
        painter->fillRect(QRect(0, y - 2, width / 3.0 * 2, pkh + 4), Qt::black);
        painter->fillRect(QRect(0, y, width / 3.0 * 1.5, pkh), QColor(48, 48, 48));
        break;

      case 11:
        h.text(QRect(0, y, width - h.config()->defaultTextMargin(), pkh - 2), "C" + QString::number(Sft::LAST_OCTAVE + Sft::FIRST_OCTAVE - topOctave, 10), h.color(QPalette::WindowText), RenderTheme::FontDefault, Qt::AlignVCenter | Qt::AlignRight);
        break;
    }

    // NEXT ITEM
    y += pkh;
    if (++topNote == Sft::NUM_NOTES_PER_OCTAVE) {
      topNote = 0;
      
      if (topOctave++ == Sft::LAST_OCTAVE) {
        break;
      }
    }
  }
}

// -----------------------------------------------------------------------------
bool PianoRollPiano::mouseMoveEvent(const QPoint &p) {
  if (!mMouseIsDown) {
    return false;
  }

  int note = Sft::NUM_NOTES_PER_OCTAVE * Sft::NUM_OCTAVES - 1 - ((p.y() + mScrollTop) / config()->pianoKeyHeight());
  if (note == mLastNote) {
    return true;
  }

  mLastNote = note;
  Sft::InstrumentPtr instrument;
  int channel = 7;
  if (mContext) {
    instrument = mContext->activeInstrument();
    channel = mContext->activeChannel()->index();
  }
  if (instrument) {
    AudioPlayer::instance()->play(instrument, note, 1, channel);
  }

  return true;
}

// -----------------------------------------------------------------------------
bool PianoRollPiano::mousePressEvent(Qt::MouseButton button, const QPoint &p) {
  if (button != Qt::LeftButton) {
    return false;
  }

  mMouseIsDown = true;
  mouseMoveEvent(p);
  return true;
}

// -----------------------------------------------------------------------------
bool PianoRollPiano::mouseReleaseEvent(Qt::MouseButton button, const QPoint &) {
  if (button != Qt::LeftButton) {
    return false;
  }

  mMouseIsDown = false;
  mLastNote = -1;
  return true;

}