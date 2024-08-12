#pragma once

#include <QPoint>

namespace Editor {
class PianoRollNoteEditor;
class RenderHelper;

class NoteEditorTool {
public:
  //! Constructor
  NoteEditorTool(PianoRollNoteEditor *editor) : mEditor(editor) {}

  //! Deconstructor
  virtual ~NoteEditorTool() = default;

  //! Mouse event
  virtual bool mousePress(Qt::MouseButton, const QPoint &) { return false; }

  //! Mouse event
  virtual bool mouseMove(const QPoint &) { return false; }

  //! Mouse event
  virtual bool mouseRelease(Qt::MouseButton, const QPoint &) { return true; }

  //! Cancel the event
  virtual void cancel() {}

  //! Draws the current instrument
  virtual void draw(RenderHelper&, int, int, int, int) {}

protected:
  //! The editor
  PianoRollNoteEditor *mEditor;
};

}