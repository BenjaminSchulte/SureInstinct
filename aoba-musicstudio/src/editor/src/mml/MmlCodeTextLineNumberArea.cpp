#include <editor/mml/MmlCodeTextLineNumberArea.hpp>
#include <editor/mml/MmlCodeTextEdit.hpp>

using namespace Editor;

// ----------------------------------------------------------------------------
MmlCodeTextLineNumberArea::MmlCodeTextLineNumberArea(MmlCodeTextEdit *editor)
  : QWidget(editor)
  , mEditor(editor)
{
}

// ----------------------------------------------------------------------------
void MmlCodeTextLineNumberArea::paintEvent(QPaintEvent *event) { 
  mEditor->lineNumberAreaPaintEvent(event);
}

// ----------------------------------------------------------------------------
QSize MmlCodeTextLineNumberArea::sizeHint() const {
  return QSize(mEditor->lineNumberAreaWidth(), 0);
}