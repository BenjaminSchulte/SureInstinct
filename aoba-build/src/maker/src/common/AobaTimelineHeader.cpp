#include <QHBoxLayout>
#include <aoba/log/Log.hpp>
#include <QToolButton>
#include <maker/common/AobaTimelineHeader.hpp>

// -----------------------------------------------------------------------------
AobaTimelineHeader::AobaTimelineHeader(QWidget *parent)
  : QWidget(parent)
  , mLayout(new QHBoxLayout(this))
{
  mLayout->setMargin(0);
  mLayout->setSpacing(0);
  setLayout(mLayout);

  mStart = new QToolButton(this);
  mPrevious = new QToolButton(this);
  mNext = new QToolButton(this);
  mEnd = new QToolButton(this);
  mAdd = new QToolButton(this);
  mRemove = new QToolButton(this);

  mStart->setIcon(QIcon(":/bright/track-start"));
  mPrevious->setIcon(QIcon(":/bright/track-prev"));
  mNext->setIcon(QIcon(":/bright/track-next"));
  mEnd->setIcon(QIcon(":/bright/track-end"));
  mAdd->setIcon(QIcon(":/bright/plus"));
  mRemove->setIcon(QIcon(":/bright/cross"));

  mLayout->addWidget(mStart);
  mLayout->addWidget(mPrevious);
  mLayout->addWidget(mNext);
  mLayout->addWidget(mEnd);
  mLayout->addWidget(mAdd);
  mLayout->addWidget(mRemove);

  connect(mStart, &QToolButton::clicked, this, &AobaTimelineHeader::on_mStart_triggered);
  connect(mPrevious, &QToolButton::clicked, this, &AobaTimelineHeader::on_mPrevious_triggered);
  connect(mNext, &QToolButton::clicked, this, &AobaTimelineHeader::on_mNext_triggered);
  connect(mEnd, &QToolButton::clicked, this, &AobaTimelineHeader::on_mEnd_triggered);
  connect(mAdd, &QToolButton::clicked, this, &AobaTimelineHeader::addClicked);
  connect(mRemove, &QToolButton::clicked, this, &AobaTimelineHeader::removeClicked);

  mLayout->addStretch();
  setNumFrames(0);
}

// -----------------------------------------------------------------------------
void AobaTimelineHeader::on_mStart_triggered() {
  emit frameChangeRequested(mNumFrames >= 0 ? 0 : -1);
}

// -----------------------------------------------------------------------------
void AobaTimelineHeader::on_mPrevious_triggered() {
  emit frameChangeRequested(mNumFrames >= 0 ? qMax(mCurrentFrame - 1, 0) : -1);
}

// -----------------------------------------------------------------------------
void AobaTimelineHeader::on_mNext_triggered() {
  emit frameChangeRequested(mNumFrames >= 0 ? qMin(mCurrentFrame + 1, mNumFrames - 1) : -1);
}

// -----------------------------------------------------------------------------
void AobaTimelineHeader::on_mEnd_triggered() {
  emit frameChangeRequested(mNumFrames - 1);
}


// -----------------------------------------------------------------------------
void AobaTimelineHeader::setNumFrames(int numFrames) {
  mNumFrames = numFrames;

  if (numFrames == 0) {
    setCurrentFrame(-1);
  } else {
    setCurrentFrame(mCurrentFrame % numFrames);
  }
}

// -----------------------------------------------------------------------------
void AobaTimelineHeader::setCurrentFrame(int frame) {
  bool first = (frame <= 0);
  bool last = (frame == -1 || frame == mNumFrames - 1);

  mStart->setEnabled(!first);
  mPrevious->setEnabled(!first);
  mEnd->setEnabled(!last);
  mNext->setEnabled(!last);
  mRemove->setEnabled(frame != -1);
  mCurrentFrame = frame;
}