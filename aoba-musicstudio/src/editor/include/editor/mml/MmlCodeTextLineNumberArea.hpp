#pragma once

#include <QWidget>

namespace Editor {

class MmlCodeTextEdit;

class MmlCodeTextLineNumberArea : public QWidget {
  Q_OBJECT

public:
  MmlCodeTextLineNumberArea(MmlCodeTextEdit *editor);

  QSize sizeHint() const override;

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  MmlCodeTextEdit *mEditor;
};

}