#pragma once

#include <QVector>
#include <QScrollArea>

class QVBoxLayout;
class AobaTimelineTrack;
class AobaTimelineItem;
class AobaTimelineHeader;

class AobaTimeline : public QScrollArea {
  Q_OBJECT

public:
  //! Constructor
  AobaTimeline(QWidget *parent = nullptr);

  //! Clears all tracks
  void clear();

  //! Creates a new track
  AobaTimelineTrack *createTrack(const QString &name);

  //! Returns the active track
  inline AobaTimelineTrack *activeTrack() const { return (mActiveTrack == -1 ? nullptr : mTracks[mActiveTrack]); }

  //! Returns the current item
  AobaTimelineItem *currentItem() const;

  //! Returns the header
  inline AobaTimelineHeader *header() const { return mHeader; }

  //! Returns the frame index
  int frameIndexOf(const AobaTimelineItem *) const;

signals:
  //! The current track has changed
  void activeTrackChanged(int);

  //! The current item changed
  void currentItemChanged(AobaTimelineItem*);

public slots:
  //! Sets the current index
  void setCurrentFrameIndex(int index);

private slots:
  //! Slot item changed
  void onTrackCurrentItemChanged(AobaTimelineItem*);

  //! TRacks changed
  void onTrackChanged();

private:
  //! The widget
  QWidget *mWidget;

  //! The layout
  QVBoxLayout *mLayout;

  //! The header
  AobaTimelineHeader *mHeader;

  //! List of all track
  QVector<AobaTimelineTrack*> mTracks;

  //! The active track index
  int mActiveTrack = -1;
};