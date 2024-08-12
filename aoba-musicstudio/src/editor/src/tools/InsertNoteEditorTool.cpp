#include <QDebug>
#include <editor/tools/InsertNoteEditorTool.hpp>
#include <editor/render/RenderWidget.hpp>
#include <editor/project/ProjectContext.hpp>
#include <editor/playback/AudioPlayer.hpp>
//#include <sft/sequence/Note.hpp>
#include <sft/Configuration.hpp>

using namespace Editor;

// -----------------------------------------------------------------------------
bool InsertNoteEditorTool::mousePress(Qt::MouseButton, const QPoint &p) {
  if (!mEditor->context() || mEditor->context()->activeInstrument() == nullptr) {
    return false;
  }

  (void)p;
  //mNoteCreateStart = mEditor->noteAt(p);
  mNoteCreateDuration = Sft::NUM_TICK_SUBTICKS;
  mEditor->renderWidget()->update();
  return true;
}

// -----------------------------------------------------------------------------
bool InsertNoteEditorTool::mouseMove(const QPoint &p) {
  /*uint32_t stepSize = Sft::NUM_TICK_SUBTICKS;

  auto position = mEditor->noteAt(p);
  uint32_t newDuration = qMax<int>(stepSize, ((int)position.absoluteTick() - (int)mNoteCreateStart.absoluteTick() + 1) * stepSize);

  if (newDuration != mNoteCreateDuration) {
    mNoteCreateDuration = newDuration;
    mEditor->renderWidget()->update();
  }*/
  (void)p;

  return true;
}

// -----------------------------------------------------------------------------
void InsertNoteEditorTool::cancel() {
  if (!mEditor->context()) {
    return;
  }

  //Sft::Note note(mNoteCreateStart.note(), mEditor->context()->activeInstrument());
  //AudioPlayer::instance()->play(note);
}

// -----------------------------------------------------------------------------
void InsertNoteEditorTool::draw(RenderHelper &h, int sx, int sy, int leftTick, int topNote) {
  if (!mEditor->context() || !mEditor->context()->activeInstrument()) {
    return;
  }

  (void)h;
  (void)sx;
  (void)sy;
  (void)leftTick;
  (void)topNote;
  //Sft::Note note(mNoteCreateStart.note(), mEditor->context()->activeInstrument());
  //mEditor->drawNote(h, note, sx, sy, mNoteCreateStart.absoluteTick() - leftTick, topNote);
}
