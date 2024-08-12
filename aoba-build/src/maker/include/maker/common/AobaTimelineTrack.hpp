#pragma once

#include <QWidget>
#include <QVector>
#include <QSignalMapper>

class QSpacer;
class QHBoxLayout;
class AobaTimeline;
class AobaTimelineItem;

class AobaTimelineTrack : public QWidget {
  Q_OBJECT

public:
  //! Constructor
  AobaTimelineTrack(QWidget *parent);

  //! Clears the track
  void clear();

  //! Adds an item
  void addItem(AobaTimelineItem *);

  //! Adds an item
  void insertItem(int index, AobaTimelineItem *, bool adjustItems=true);

  //! Adds an item
  void removeItem(AobaTimelineItem *, bool adjustItems=true);

  //! Returns the current item
  AobaTimelineItem *currentItem() const;

  //! Returns an item
  AobaTimelineItem *itemAtFrameIndex(int index) const;

  //! Returns the frame index
  int frameIndexOf(const AobaTimelineItem *) const;

  //! Returns the number of frame indices
  inline int numFrameIndices() const { return mItems.count(); }

signals:
  //! Current item changed
  void currentItemChanged(AobaTimelineItem *);

  //! Item created
  void itemAdded(AobaTimelineItem*);

  //! Item created
  void itemRemoved(AobaTimelineItem*);

public slots:
  //! Sets the current index
  void setCurrentFrameIndex(int);

private slots:
  //! On item click
  void onItemActivate(QWidget *);

private:
  //! The layout
  QHBoxLayout *mLayout;

  //! The current item
  AobaTimelineItem *mCurrentItem = nullptr;

  //! List of all items
  QVector<AobaTimelineItem*> mItems;

  //! Signal mapper
  QSignalMapper mSignals;
};
