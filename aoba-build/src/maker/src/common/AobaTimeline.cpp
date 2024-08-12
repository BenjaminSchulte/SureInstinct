#include <aoba/log/Log.hpp>
#include <QVBoxLayout>
#include <maker/common/AobaTimeline.hpp>
#include <maker/common/AobaTimelineHeader.hpp>
#include <maker/common/AobaTimelineTrack.hpp>
#include <maker/common/AobaTimelineItem.hpp>

// -----------------------------------------------------------------------------
AobaTimeline::AobaTimeline(QWidget *parent)
  : QScrollArea(parent)
  , mWidget(new QWidget(this))
  , mLayout(new QVBoxLayout(mWidget))
  , mHeader(new AobaTimelineHeader(mWidget))
{
  setWidget(mWidget);

  mLayout->addWidget(mHeader);

  connect(mHeader, &AobaTimelineHeader::frameChangeRequested, this, &AobaTimeline::setCurrentFrameIndex);

  widget()->setLayout(mLayout);
}

// -----------------------------------------------------------------------------
AobaTimelineItem *AobaTimeline::currentItem() const {
  if (mActiveTrack == -1) {
    return nullptr;
  }

  return mTracks[mActiveTrack]->currentItem();
}

// -----------------------------------------------------------------------------
void AobaTimeline::onTrackCurrentItemChanged(AobaTimelineItem *item) {
  mHeader->setCurrentFrame(frameIndexOf(item));

  emit currentItemChanged(currentItem());
}

// -----------------------------------------------------------------------------
void AobaTimeline::onTrackChanged() {
  if (mActiveTrack == -1) {
    return;
  }

  mHeader->setNumFrames(mTracks[mActiveTrack]->numFrameIndices());
  mHeader->setCurrentFrame(mTracks[mActiveTrack]->frameIndexOf(mTracks[mActiveTrack]->currentItem()));
}

// -----------------------------------------------------------------------------
int AobaTimeline::frameIndexOf(const AobaTimelineItem *item) const {
  if (!item) {
    return -1;
  }

  return item->frameIndex();
}

// -----------------------------------------------------------------------------
void AobaTimeline::setCurrentFrameIndex(int index) {
  if (mActiveTrack == -1) {
    return;
  }

  mTracks[mActiveTrack]->setCurrentFrameIndex(index);
}

// -----------------------------------------------------------------------------
void AobaTimeline::clear() {
  for (AobaTimelineTrack *track : mTracks) {
    track->clear();
  }

  mHeader->setNumFrames(0);
  mHeader->setCurrentFrame(-1);
}

// -----------------------------------------------------------------------------
AobaTimelineTrack *AobaTimeline::createTrack(const QString &) {
  AobaTimelineTrack *track = new AobaTimelineTrack(widget());
  connect(track, &AobaTimelineTrack::currentItemChanged, this, &AobaTimeline::onTrackCurrentItemChanged);
  connect(track, &AobaTimelineTrack::itemAdded, this, &AobaTimeline::onTrackChanged);
  connect(track, &AobaTimelineTrack::itemRemoved, this, &AobaTimeline::onTrackChanged);
  mTracks.push_back(track);
  mLayout->addWidget(track);

  if (mActiveTrack == -1) {
    mActiveTrack = 0;
    emit activeTrackChanged(mActiveTrack);
  }

  return track;
}