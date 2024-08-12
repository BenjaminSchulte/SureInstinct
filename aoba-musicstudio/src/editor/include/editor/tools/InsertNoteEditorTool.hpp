#pragma once

#include "NoteEditorTool.hpp"
#include "../piano/PianoRollNoteEditor.hpp"

namespace Editor {

class InsertNoteEditorTool : public NoteEditorTool {
public:
  //! Constructor
  InsertNoteEditorTool(PianoRollNoteEditor *editor) : NoteEditorTool(editor) {}

  //! Mouse event
  bool mousePress(Qt::MouseButton, const QPoint &p) override;

  //! Mouse event
  bool mouseMove(const QPoint &p) override;

  //! Cancel the event
  void cancel() override;

  //! Draws the current instrument
  void draw(RenderHelper&, int, int, int, int) override;

protected:
  //! The start position for the node creation
  PianoRollNoteEditor::LocationInfo mNoteCreateStart;

  //! The current note duration
  uint32_t mNoteCreateDuration;
};

}