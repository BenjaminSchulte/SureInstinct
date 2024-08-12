#include <QHBoxLayout>
#include <aoba/log/Log.hpp>
#include <maker/common/AobaTimelineTrack.hpp>
#include <maker/common/AobaTimelineItem.hpp>

// -----------------------------------------------------------------------------
AobaTimelineTrack::AobaTimelineTrack(QWidget *parent)
  : QWidget(parent)
  , mLayout(new QHBoxLayout(this))
{
  mLayout->setMargin(0);
  mLayout->setSpacing(0);
  setLayout(mLayout);

  mLayout->addStretch();

  connect(&mSignals, SIGNAL(mapped(QWidget*)), this, SLOT(onItemActivate(QWidget*)));

  clear();
}

// -----------------------------------------------------------------------------
void AobaTimelineTrack::clear() {
  for (AobaTimelineItem *item : mItems) {
    mSignals.removeMappings(item);
    delete item;
  }
  mItems.clear();

  mCurrentItem = nullptr;
  emit currentItemChanged(nullptr);
}

// -----------------------------------------------------------------------------
void AobaTimelineTrack::onItemActivate(QWidget *obj) {
  AobaTimelineItem *item = dynamic_cast<AobaTimelineItem*>(obj);
  bool didChange = (item != mCurrentItem) || obj == nullptr;

  if (mCurrentItem) {
    mCurrentItem->setChecked(false);
  }

  mCurrentItem = item;

  if (mCurrentItem) {
    mCurrentItem->setChecked(true);
  }

  if (didChange) {
    emit currentItemChanged(item);
  }
}

// -----------------------------------------------------------------------------
AobaTimelineItem *AobaTimelineTrack::currentItem() const {
  return mCurrentItem;
}

// -----------------------------------------------------------------------------
AobaTimelineItem *AobaTimelineTrack::itemAtFrameIndex(int index) const {
  if (index < 0 || index >= mItems.size()) {
    return nullptr;
  }

  return mItems[index];
}

// -----------------------------------------------------------------------------
void AobaTimelineTrack::addItem(AobaTimelineItem *item) {
  insertItem(mItems.size(), item, false);
}

// -----------------------------------------------------------------------------
void AobaTimelineTrack::insertItem(int index, AobaTimelineItem *item, bool adjustItems) {
  mItems.insert(index, item);
  mLayout->insertWidget(index, item);

  connect(item, SIGNAL(clicked(bool)), &mSignals, SLOT(map()));
  connect(item, SIGNAL(pressed()), &mSignals, SLOT(map()));
  mSignals.setMapping(item, item);

  if (adjustItems) {
    for (int i=index+1; i<mItems.size(); i++) {
      mItems[i]->setStart(mItems[i]->start() + item->duration());
    }
  }

  emit itemAdded(item);

  if (mCurrentItem == nullptr) {
    mCurrentItem = item;
    emit currentItemChanged(item);
    mCurrentItem->setChecked(true);
  }
}

// -----------------------------------------------------------------------------
void AobaTimelineTrack::removeItem(AobaTimelineItem *item, bool adjustItems) {
  int index = frameIndexOf(item);
  if (index < 0) {
    return;
  }

  mItems.remove(index);

  if (adjustItems) {
    for (int i=index; i<mItems.size(); i++) {
      mItems[i]->setStart(mItems[i]->start() - item->duration());
    }
  }

  if (mCurrentItem == item) {
    mCurrentItem = nullptr;

    AobaTimelineItem *newItem;
    if (index < mItems.size()) {
      newItem = mItems[index];
    } else if (!mItems.isEmpty()) {
      newItem = mItems.last();
    } else {
      newItem = nullptr;
    }

    onItemActivate(newItem);
  }

  emit itemRemoved(item);
  delete item;
}

// -----------------------------------------------------------------------------
int AobaTimelineTrack::frameIndexOf(const AobaTimelineItem *item) const {
  return mItems.indexOf(const_cast<AobaTimelineItem*>(item));
}

// -----------------------------------------------------------------------------
void AobaTimelineTrack::setCurrentFrameIndex(int index) {
  if (index < 0 || index >= mItems.size()) {
    onItemActivate(nullptr);
  } else {
    onItemActivate(mItems[index]);
  }
}