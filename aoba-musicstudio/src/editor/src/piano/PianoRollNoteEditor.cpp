#include <QDebug>
#include <QPainterPath>
#include <QPainter>
#include <editor/application/DisplayConfiguration.hpp>
#include <editor/piano/PianoRoll.hpp>
#include <editor/piano/PianoRollNoteEditor.hpp>
#include <editor/render/RenderHelper.hpp>
#include <editor/render/RenderWidget.hpp>
#include <editor/project/ProjectContext.hpp>
#include <editor/project/Project.hpp>
#include <editor/tools/InsertNoteEditorTool.hpp>
#include <sft/song/Song.hpp>
#include <sft/instrument/Instrument.hpp>
#include <sft/mml/MmlExecutedScript.hpp>
#include <sft/mml/MmlPlayNoteCommand.hpp>
#include <sft/mml/MmlSetTempoCommand.hpp>
#include <sft/mml/MmlScript.hpp>
#include <sft/Configuration.hpp>

using namespace Editor;

// -----------------------------------------------------------------------------
PianoRollNoteEditor::~PianoRollNoteEditor() {
  delete mCurrentEditTool;
}

// -----------------------------------------------------------------------------
void PianoRollNoteEditor::draw(RenderHelper &h) {
  //int top = -mPosition.top();
  int bottom = mPosition.bottom();
  //int width = mPosition.width();
  int pkh = h.config()->pianoKeyHeight();
  int mtw = h.config()->measureTickWidth() * mZoom;
  mKeyHeight = pkh;
  mMeasureTickWidth = mtw;

  int topNoteTotal = mScrollTop / pkh;
  int leftTickTotal = mScrollLeft / mtw;
  //int topOctave = topNoteTotal / Sft::NUM_NOTES_PER_OCTAVE + Sft::FIRST_OCTAVE;
  int topNote = topNoteTotal % Sft::NUM_NOTES_PER_OCTAVE;
  int leftTick = leftTickTotal % (Sft::MEASURE_DURATION / Sft::MEASURE_TICK_SUBDIVISION);

  QRect full(0, 0, mPosition.right(), bottom);
  QPainter *p = h.painter();
  int sy = -(mScrollTop % pkh) + mPosition.top();
  int sx = -(mScrollLeft % mtw) + mPosition.left();

  // Draw background rows
  drawRows(h, p, topNote, sy, pkh, bottom);
  drawCols(h, p, leftTick, sx, mtw);

  Sft::Song *song = (mContext ? mContext->project()->song() : nullptr);
  if (song) {
    for (uint8_t i=0; i<song->numChannels(); i++) {
      Sft::Channel *channel = song->channel(i);
      
      if (channel != mChannel) {
        drawNotes(h, p, channel, leftTickTotal, topNoteTotal, mtw, sx, sy, true);
      }
    }
  }
  drawNotes(h, p, mChannel, leftTickTotal, topNoteTotal, mtw, sx, sy, false);

  if (mCurrentEditTool) {
    mCurrentEditTool->draw(h, sx, sy, leftTickTotal, topNoteTotal);
  }

  if (mContext) {
    mContext->setEditorWidth((mPosition.width() - mScrollLeft) / (float)mtw * (float)Sft::MEASURE_TICK_SUBDIVISION);
  }
}

// -----------------------------------------------------------------------------
void PianoRollNoteEditor::drawNotes(RenderHelper &h, QPainter *p, Sft::Channel *channel, int leftTick, int topNote, int mtw, int sx, int sy, bool alpha) {
  const auto *executed = channel->script()->executed();
  if (!executed) {
    return;
  }

  int index = executed->commandIndexAtTick(leftTick * Sft::MEASURE_TICK_SUBDIVISION);
  int numCommands = executed->numCommands();
  if (index < 0) {
    return;
  }

  unsigned rightTick = (mPosition.width() / mtw + 1) * Sft::MEASURE_TICK_SUBDIVISION;


  while (index < numCommands) {
    const Sft::MmlExecutedCommand &command = executed->command(index++);
    if (command.state().tick >= rightTick) {
      break;
    }
    
    switch (command.command()->type()) {
      case Sft::MmlCommandType::PLAY_NOTE:
        drawNote(h, p, command, (int)command.state().tick - leftTick * Sft::MEASURE_TICK_SUBDIVISION, topNote, sx, sy, alpha);
        break;

      case Sft::MmlCommandType::SET_TEMPO:
        drawCommand(h, p, QString("%1 BPM").arg(command.command().dynamicCast<Sft::MmlSetTempoCommand>()->bpm()), (int)command.state().tick - leftTick * Sft::MEASURE_TICK_SUBDIVISION, sx);
        break;

      default:
        break;
    }
  }
}

// -----------------------------------------------------------------------------
void PianoRollNoteEditor::drawCols(RenderHelper &h, QPainter *p, int tick, int sx, int mtw) {
  bool first = true;

  for (int x=sx; x<mPosition.right(); x+=mtw) {
    int subTick = tick % Sft::MEASURE_TICKS;

    if (tick == 0) {
      p->setPen(QPen(QColor(Qt::darkGray).darker(140)));
    } else if (subTick == 0) {
      p->setPen(QPen(Qt::black));
    } else if (tick == 1 || subTick == 1 || first) {
      p->setPen(QPen(h.color(QPalette::Base).darker(120)));
    }

    p->drawLine(QPoint(x, mPosition.top()), QPoint(x, mPosition.bottom()));

    if (++tick == Sft::MEASURE_DURATION / Sft::MEASURE_TICK_SUBDIVISION) {
      tick = 0;
    }

    first = false;
  }
}

// -----------------------------------------------------------------------------
void PianoRollNoteEditor::drawRows(RenderHelper &h, QPainter *p, int note, int sy, int pkh, int bottom) {
  QBrush lightRowBrush(h.color(QPalette::AlternateBase));
  p->setPen(QPen(h.color(QPalette::Base).darker(120)));

  for (int y=sy; y<bottom; y+=pkh) {
    switch (note) {
      case 0:
      case 2:
      case 4:
      case 6:
      case 7:
      case 9:
      case 11:
        p->fillRect(QRect(mPosition.left(), y, mPosition.width(), pkh), lightRowBrush);
        break;
    }

    p->drawLine(QPoint(mPosition.left(), y), QPoint(mPosition.right(), y));

    note = (note + 1) % Sft::NUM_NOTES_PER_OCTAVE;
  }
}

// -----------------------------------------------------------------------------
void PianoRollNoteEditor::drawCommand(RenderHelper &h, QPainter *p, const QString &command, int tick, int sx) {
  int mtw = h.config()->measureTickWidth() * mZoom;
  double smtw = (double)mtw / Sft::MEASURE_TICK_SUBDIVISION;

  int top = mPosition.top();
  int left = tick * smtw + sx + 1;

  QFont font(h.font(Editor::RenderTheme::FontDefault));
  QFontMetrics fm(font);
  int width = fm.horizontalAdvance(command) + 10;
  int height = fm.height();

  p->setPen(QPen(QColor(255, 192, 0)));
  p->fillRect(left, top, width, height, QBrush(QColor(255, 192, 0)));
  p->drawLine(left, top, left, mPosition.bottom());
  p->setPen(QPen(QColor(0, 0, 0)));
  p->setFont(font);
  p->drawText(QPoint(left + 5, top + height - 3), command);
}

// -----------------------------------------------------------------------------
void PianoRollNoteEditor::drawNote(RenderHelper &h, QPainter *p, const Sft::MmlExecutedCommand &command, int tick, int topNote, int sx, int sy, bool alpha) {
  QSharedPointer<Sft::MmlPlayNoteCommand> note = command.command().dynamicCast<Sft::MmlPlayNoteCommand>();
  if (!note) {
    return;
  }

  int mtw = h.config()->measureTickWidth() * mZoom;
  double smtw = (double)mtw / Sft::MEASURE_TICK_SUBDIVISION;
  int actualNote = Sft::NUM_OCTAVES * Sft::NUM_NOTES_PER_OCTAVE - 1 - note->note() - command.state().octave * Sft::NUM_NOTES_PER_OCTAVE;
  int pkh = h.config()->pianoKeyHeight();
  int volumePkh = (command.state().volume + 64) / 127.0 * (double)pkh;
  unsigned duration = note->duration();

  double left = tick * smtw + sx;
  double top = (actualNote - topNote) * pkh + pkh / 2 - volumePkh / 2 + sy - command.state().pitchOffset * pkh;
  double centerY = top + volumePkh * 0.5;

  QColor color(255, 255, 255);
  if (command.state().instrument) {
    color = h.color(command.state().instrument->color());
  }

  const auto &pitch = command.state().pitch;
  if (pitch.notes) {
    double pitchDirection = (pitch.notes < 0 ? -1 : 1);
    double pitchNotes = pitch.notes + pitchDirection * 0.5;
    double pitchDuration = pitch.duration;

    if (pitchDuration > duration) {
      double factor = duration / pitchDuration;
      pitchDuration *= factor;
      pitchNotes *= factor;
    }

    QPainterPath path;
    path.moveTo(left, centerY - volumePkh * pitchDirection * 0.5);
    path.lineTo(left + pitchDuration * smtw, centerY - pitchNotes * pkh);
    path.lineTo(left + duration * smtw, centerY - pitchNotes * pkh);
    path.lineTo(left + duration * smtw, centerY - volumePkh * pitchDirection * 0.5);
    p->setOpacity(alpha ? 0.3 : 1.0);
    p->setPen(Qt::NoPen);
    p->fillPath(path, QBrush(color));
    p->setOpacity(1.0);
  }

  h.listItemBackground(QRect(left, top, duration * smtw, volumePkh), color, alpha ? 0.3 : 1.0);
}

// -----------------------------------------------------------------------------
void PianoRollNoteEditor::setCurrentEditTool(NoteEditorTool *tool) {
  if (mCurrentEditTool) {
    mCurrentEditTool->cancel();
    delete mCurrentEditTool;
  }

  mCurrentEditTool = tool;
}

// -----------------------------------------------------------------------------
bool PianoRollNoteEditor::setCurrentEditToolAndClick(NoteEditorTool *tool, Qt::MouseButton button, const QPoint &p) {
  setCurrentEditTool(tool);
  
  if (!tool->mousePress(button, p)) {
    delete tool;
    mCurrentEditTool = nullptr;
    return false;
  }

  return true;
}
   

// -----------------------------------------------------------------------------
bool PianoRollNoteEditor::mousePressEvent(Qt::MouseButton button, const QPoint &p) {
  if (button == Qt::LeftButton) {
    return setCurrentEditToolAndClick(new InsertNoteEditorTool(this), button, p);
  } else {
    return Renderable::mousePressEvent(button, p);
  }
}

// -----------------------------------------------------------------------------
bool PianoRollNoteEditor::mouseMoveEvent(const QPoint &p) {
  if (mCurrentEditTool) {
    return mCurrentEditTool->mouseMove(p);
  }

  return true;
}

// -----------------------------------------------------------------------------
bool PianoRollNoteEditor::mouseReleaseEvent(Qt::MouseButton button, const QPoint &p) {
  if (mCurrentEditTool) {
    if (mCurrentEditTool->mouseRelease(button, p)) {
      setCurrentEditTool(nullptr);
      mParent->update();
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool PianoRollNoteEditor::zoomEvent(float dir) {
  dynamic_cast<PianoRoll*>(mParent)->zoomEvent(dir);
  return true;
}