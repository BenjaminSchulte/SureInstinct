#include <QDebug>
#include <QPainter>
#include <cmath>
#include <editor/application/DisplayConfiguration.hpp>
#include <editor/sequencer/SequencerChannel.hpp>
#include <editor/render/RenderHelper.hpp>
#include <editor/render/RenderWidget.hpp>
#include <editor/project/ProjectContext.hpp>
#include <editor/project/Project.hpp>
#include <sft/song/Song.hpp>
#include <sft/channel/Channel.hpp>
#include <sft/instrument/Instrument.hpp>
#include <sft/mml/MmlScript.hpp>
#include <sft/mml/MmlExecutedScript.hpp>
#include <sft/mml/MmlPlayNoteCommand.hpp>
#include <editor/playback/AudioManager.hpp>
#include <editor/playback/AudioSongProcessor.hpp>

using namespace Editor;

// -----------------------------------------------------------------------------
SequencerChannel::SequencerChannel(RenderWidget *parent, const QRect &rect, ProjectContext *project, int channel)
  : Renderable(parent, rect)
  , mContext(project)
  , mChannel(channel)
{
}

// -----------------------------------------------------------------------------
void SequencerChannel::draw(RenderHelper &h) {
  QPainter *p = h.painter();

  AudioSongProcessor *processor = AudioManager::instance()->songProcessor();

  bool active = (mContext->activeChannel() == mContext->project()->song()->channel(mChannel));

  QBrush lightRowBrush(h.color((mChannel % 2) == 0 ? QPalette::Base : QPalette::AlternateBase));
  if (active) {
    lightRowBrush = QBrush(QColor(96, 112, 128));
  }

  p->fillRect(mPosition, lightRowBrush);
  p->setPen(QPen(h.color(QPalette::Base).darker(120)));
  p->drawLine(QPoint(mPosition.left(), mPosition.bottom()), QPoint(mPosition.right(), mPosition.bottom()));

  double measureWidth = Sft::MEASURE_DURATION * mZoom;
  double measureLeft = -std::fmod(mScrollX, measureWidth);
  p->setPen(QPen(QColor(0, 0, 0)));
  while (measureLeft < mPosition.width()) {
    p->drawLine(measureLeft, mPosition.top(), measureLeft, mPosition.bottom());
    measureLeft += measureWidth;
  }

  if (active && mContext) {
    int windowLeft = (mContext->editorScroll() - mPosition.left()) * mZoom - mScrollX;
    int windowWidth = mContext->editorWidth() * mZoom;
    p->setPen(QPen(QColor(100, 200, 0), 1, Qt::DotLine));
    p->drawRect(windowLeft, mPosition.top(), windowWidth, mPosition.height() - 1);
  }

  int currentTick = (processor->channelTick(mChannel) - mPosition.left()) * mZoom - mScrollX;
  int repeatTick = (processor->repeatTick() - mPosition.left()) * mZoom - mScrollX;
  p->setPen(QPen(QColor(0, 255, 0)));
  p->drawLine(repeatTick, mPosition.top(), repeatTick, mPosition.bottom());
  p->drawLine(repeatTick + 1, mPosition.top(), repeatTick + 1, mPosition.bottom());
  p->setPen(QPen(QColor(255, 255, 0)));
  p->drawLine(currentTick, mPosition.top(), currentTick, mPosition.bottom());
  p->drawLine(currentTick + 1, mPosition.top(), currentTick + 1, mPosition.bottom());

  drawNotes(h);
}

// -----------------------------------------------------------------------------
void SequencerChannel::drawNotes(RenderHelper &h) const {
  if (!mContext) {
    return;
  }

  Sft::MmlExecutedScript *executed = mContext->project()->song()->channel(mChannel)->script()->executed();
  if (!executed) {
    return;
  }

  unsigned leftTick = mScrollX / mZoom;
  unsigned rightTick = leftTick + mPosition.width() / mZoom;

  int index = executed->commandIndexAtTick(leftTick);
  int numCommands = executed->numCommands();
  if (index < 0) {
    return;
  }

  while (index < numCommands) {
    const Sft::MmlExecutedCommand &command = executed->command(index++);
    if (command.state().tick >= rightTick) {
      break;
    }

    switch (command.command()->type()) {
      case Sft::MmlCommandType::PLAY_NOTE:
        drawNote(h, command, (int)command.state().tick - mScrollX / mZoom);
        break;

      default:
        break;
    }
  }
}

// -----------------------------------------------------------------------------
void SequencerChannel::drawNote(RenderHelper &h, const Sft::MmlExecutedCommand &command, int offset) const {
  QSharedPointer<Sft::MmlPlayNoteCommand> note = command.command().dynamicCast<Sft::MmlPlayNoteCommand>();
  const Sft::InstrumentPtr &instr = command.state().instrument;
  if (!note) {
    return;
  }

  QColor color(255, 255, 255);
  if (instr) {
    color = h.color(instr->color());
  }

  Sft::MmlExecutedScript *executed = mContext->project()->song()->channel(mChannel)->script()->executed();
  int topNote = executed->highestNote() + 4;
  int bottomNote = executed->lowestNote() - 4;
  double pixelPerNote = mPosition.height() / (double)(topNote - bottomNote);
  int actualNote = note->note() + command.state().octave * Sft::NUM_NOTES_PER_OCTAVE;
  double y = mPosition.bottom() - (actualNote - bottomNote) * pixelPerNote;

  QPainter *p = h.painter();
  p->setPen(QPen(color));
  p->drawLine(QPoint(offset * mZoom, y - 0.5), QPoint((offset + (int)note->duration()) * mZoom, y - 0.5));
  p->drawLine(QPoint(offset * mZoom, y + 0.5), QPoint((offset + (int)note->duration()) * mZoom, y + 0.5));
}

// -----------------------------------------------------------------------------
bool SequencerChannel::mousePressEvent(Qt::MouseButton button, const QPoint &p) {
  if (button == Qt::LeftButton) {
    mContext->setActiveChannel(mContext->project()->song()->channel(mChannel));
    mMouseSetsEditorPosition = true;
    mouseMoveEvent(p);
  } else if (button == Qt::RightButton) {
    AudioManager::instance()->songProcessor()->setRepeatTick((p.x() + mScrollX) / mZoom);
    mParent->update();
  } else {
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
bool SequencerChannel::mouseMoveEvent(const QPoint &p) {
  if (!mMouseSetsEditorPosition || !mContext) {
    return false;
  }

  float note = qMax<float>(0, ((p.x() + mScrollX) / mZoom) - mContext->editorWidth() / 2);
  mContext->setEditorScroll(note);
  return true;
}

// -----------------------------------------------------------------------------
bool SequencerChannel::mouseReleaseEvent(Qt::MouseButton button, const QPoint &) {
  if (button == Qt::LeftButton) {
    mMouseSetsEditorPosition = false;
  }

  return true;
}
