#pragma once

#include <QWidget>
#include <QVector>
#include <QSignalMapper>

class QToolButton;
class QHBoxLayout;

class AobaTimelineHeader : public QWidget {
  Q_OBJECT

public:
  //! Constructor
  AobaTimelineHeader(QWidget *parent);

signals:
  void frameChangeRequested(int);
  void addClicked();
  void removeClicked();

public slots:
  //! Sets the number of frames
  void setNumFrames(int numFrames);

  //! Sets the current frame
  void setCurrentFrame(int frame);

private slots:
  void on_mStart_triggered();
  void on_mPrevious_triggered();
  void on_mNext_triggered();
  void on_mEnd_triggered();

private:
  //! The layout
  QHBoxLayout *mLayout;

  //! The offset of the last item
  int mNumFrames;

  //! The current frame
  int mCurrentFrame;

  //! Button
  QToolButton *mStart;
  //! Button
  QToolButton *mPrevious;
  //! Button
  QToolButton *mNext;
  //! Button
  QToolButton *mEnd;
  //! Button
  QToolButton *mAdd;
  //! Button
  QToolButton *mRemove;
};
