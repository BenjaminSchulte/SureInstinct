#include <QPainter>
#include <QTextBlock>
#include <editor/mml/MmlCodeTextEdit.hpp>
#include <editor/mml/MmlCodeTextLineNumberArea.hpp>
#include <editor/application/DisplayConfiguration.hpp>

using namespace Editor;

// ----------------------------------------------------------------------------
MmlCodeTextEdit::MmlCodeTextEdit(QWidget *parent)
  : QPlainTextEdit(parent)
  , mLineNumberArea(new MmlCodeTextLineNumberArea(this))
{
  connect(this, &QPlainTextEdit::cursorPositionChanged, this, &MmlCodeTextEdit::highlightCurrentLine);
  connect(this, &QPlainTextEdit::blockCountChanged, this, &MmlCodeTextEdit::updateLineNumberAreaWidth);
  connect(this, &QPlainTextEdit::updateRequest, this, &MmlCodeTextEdit::updateLineNumberArea);

  QFont font("Monospace");
  font.setStyleHint(QFont::TypeWriter);
  setFont(font);

  updateLineNumberAreaWidth(0);
  highlightCurrentLine();
}

// ----------------------------------------------------------------------------
void MmlCodeTextEdit::resizeEvent(QResizeEvent *e) {
  QPlainTextEdit::resizeEvent(e);

  QRect cr = contentsRect();
  mLineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

// ----------------------------------------------------------------------------
void MmlCodeTextEdit::updateLineNumberAreaWidth(int) {
  setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

// ----------------------------------------------------------------------------
void MmlCodeTextEdit::highlightCurrentLine()
{
  QList<QTextEdit::ExtraSelection> extraSelections;

  if (!isReadOnly()) {
    QTextEdit::ExtraSelection selection;

    QColor lineColor = mDisplayConfiguration ? mDisplayConfiguration->theme()->palette().color(QPalette::AlternateBase) : QColor(Qt::yellow).lighter(160);

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();
    extraSelections.append(selection);
  }

  setExtraSelections(extraSelections);
}

// ----------------------------------------------------------------------------
void MmlCodeTextEdit::updateLineNumberArea(const QRect &rect, int dy) {
  if (dy) {
    mLineNumberArea->scroll(0, dy);
  } else {
    mLineNumberArea->update(0, rect.y(), mLineNumberArea->width(), rect.height());
  }

  if (rect.contains(viewport()->rect())) {
    updateLineNumberAreaWidth(0);
  }
}

// ----------------------------------------------------------------------------
void MmlCodeTextEdit::lineNumberAreaPaintEvent(QPaintEvent *event) {
  QColor background = mDisplayConfiguration ? mDisplayConfiguration->theme()->palette().color(QPalette::Window) : Qt::lightGray;
  QColor textColor = mDisplayConfiguration ? mDisplayConfiguration->theme()->palette().color(QPalette::WindowText) : Qt::black;

  QPainter painter(mLineNumberArea);
  painter.fillRect(event->rect(), background);

  QTextBlock block = firstVisibleBlock();
  int blockNumber = block.blockNumber();
  int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
  int bottom = top + (int) blockBoundingRect(block).height();

  while (block.isValid() && top <= event->rect().bottom()) {
    if (block.isVisible() && bottom >= event->rect().top()) {
      QString number = QString::number(blockNumber + 1);
      painter.setPen(textColor);
      painter.drawText(0, top, mLineNumberArea->width() - 3, fontMetrics().height(), Qt::AlignRight, number);
    }

    block = block.next();
    top = bottom;
    bottom = top + (int) blockBoundingRect(block).height();
    ++blockNumber;
  }
}

// ----------------------------------------------------------------------------
int MmlCodeTextEdit::lineNumberAreaWidth() const {
  int digits = 1;
  int max = qMax(1, blockCount());

  while (max >= 10) {
    max /= 10;
    digits++;
  }

  int space = 10 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
  return space;
}
