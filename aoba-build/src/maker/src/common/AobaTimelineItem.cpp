#include <maker/common/AobaTimelineItem.hpp>
#include <maker/common/AobaTimelineTrack.hpp>

// -----------------------------------------------------------------------------
AobaTimelineItem::AobaTimelineItem(double start, double duration, AobaTimelineTrack *parent)
  : QToolButton(parent)
  , mStart(start)
  , mDuration(duration)
  , mTrack(parent)
{
  setCheckable(true);
}

// -----------------------------------------------------------------------------
int AobaTimelineItem::frameIndex() const {
  return mTrack->frameIndexOf(this);
}