#pragma once

#include <QToolButton>

class AobaTimelineTrack;

class AobaTimelineItem : public QToolButton {
public:
  //! Constructor
  AobaTimelineItem(double start, double duration, AobaTimelineTrack *parent);

  //! Returns the offset
  inline double start() const { return mStart; }

  //! Returns the size
  inline double duration() const { return mDuration; }

  //! Sets the start
  void setStart(double newStart) { mStart = newStart; }

  //! Returns the track
  inline AobaTimelineTrack *track() const { return mTrack; }

  //! Returns the frame index
  int frameIndex() const;

private:
  //! The Start
  double mStart;

  //! The Duration
  double mDuration;

  //! The track
  AobaTimelineTrack *mTrack;
};
