#pragma once

#include <QPlainTextEdit>

namespace Editor {
class MmlCodeTextLineNumberArea;
class DisplayConfiguration;

class MmlCodeTextEdit : public QPlainTextEdit {
public:
  MmlCodeTextEdit(QWidget *parent = nullptr);

  void lineNumberAreaPaintEvent(QPaintEvent *event);
  int lineNumberAreaWidth() const;

  //! Sets the display configuration
  inline void setDisplayConfiguration(DisplayConfiguration *config) {
    mDisplayConfiguration = config;
    repaint();
    highlightCurrentLine();
  }

protected slots:
  void updateLineNumberAreaWidth(int newBlockCount);
  void highlightCurrentLine();
  void updateLineNumberArea(const QRect &, int);

protected:
  void resizeEvent(QResizeEvent *event) override;

  MmlCodeTextLineNumberArea *mLineNumberArea;

  DisplayConfiguration *mDisplayConfiguration = nullptr;
};

}