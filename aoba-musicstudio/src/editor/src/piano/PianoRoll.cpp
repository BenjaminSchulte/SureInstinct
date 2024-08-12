#include <QDebug>
#include <editor/application/DisplayConfiguration.hpp>
#include <editor/piano/PianoRoll.hpp>
#include <editor/piano/PianoRollPiano.hpp>
#include <editor/piano/PianoRollSequenceHeader.hpp>
#include <editor/piano/PianoRollNoteEditor.hpp>
#include <editor/render/RenderHelper.hpp>
#include <editor/project/Project.hpp>
#include <editor/project/ProjectContext.hpp>
#include <editor/playback/AudioManager.hpp>
#include <editor/playback/AudioSongProcessor.hpp>
#include <sft/channel/Channel.hpp>
#include <sft/mml/MmlScript.hpp>
#include <sft/mml/MmlExecutedScript.hpp>
#include <sft/Configuration.hpp>

using namespace Editor;

// -----------------------------------------------------------------------------
PianoRoll::PianoRoll(QWidget *parent)
  : RenderScrollableWidget(parent)
  , mProject(nullptr)
  , mContext(nullptr)
  , mNoteEditor(new PianoRollNoteEditor(this, QRect()))
  , mSequenceHeader(new PianoRollSequenceHeader(this, QRect()))
  , mPiano(new PianoRollPiano(this, QRect()))
{
  setMinimumSize(QSize(200, 100));

  mRenderables.push_back(mNoteEditor);
  mRenderables.push_back(mPiano);
  mRenderables.push_back(mSequenceHeader);

  connect(AudioManager::instance(), &AudioManager::onTimer, this, &PianoRoll::onAudioTimer);
}

// -----------------------------------------------------------------------------
PianoRoll::~PianoRoll() {
}

// -----------------------------------------------------------------------------
void PianoRoll::onEditorScrollChanged(float note) {
  mScrollPosition.setX(note * mConfiguration->measureTickWidth() * mZoom / Sft::MEASURE_TICK_SUBDIVISION);
  mPromoteScrollEvent = false;
  recreateContent();
  update();
  mPromoteScrollEvent = true;
}

// -----------------------------------------------------------------------------
void PianoRoll::recreateContent() {
  QSize area(size());

  int left = mConfiguration->pianoWidth();
  int top = mConfiguration->pianoHeaderHeight();
  int scrollLeft = mScrollPosition.x();
  int scrollTop = mScrollPosition.y();
  mPiano->setPosition(QRect(0, top, left, area.height() - top));
  mPiano->setScroll(scrollTop);
  mNoteEditor->setPosition(QRect(left, top, area.width() + scrollLeft - left, area.height() - top));
  mNoteEditor->setScroll(scrollLeft, scrollTop);
  mSequenceHeader->setPosition(QRect(0, 0, area.width(), top));
  mSequenceHeader->setScroll(left, scrollLeft);

  float leftNote = scrollLeft / (mConfiguration->measureTickWidth() * mZoom) * Sft::MEASURE_TICK_SUBDIVISION;
  if (mContext && mPromoteScrollEvent) {
    mContext->setEditorScroll(leftNote);
  }
}

// -----------------------------------------------------------------------------
void PianoRoll::doScroll(int, int) {
  recreateContent();
  update();
}

// -----------------------------------------------------------------------------
int PianoRoll::totalHeight() const {
  return Sft::NUM_OCTAVES * Sft::NUM_NOTES_PER_OCTAVE * mConfiguration->pianoKeyHeight();
}

// -----------------------------------------------------------------------------
bool PianoRoll::isRenderable() const {
  return RenderScrollableWidget::isRenderable() && !!mProject;
}

// -----------------------------------------------------------------------------
void PianoRoll::onAudioTimer() {
  AudioSongProcessor *processor = AudioManager::instance()->songProcessor();
  if (!processor->isPlaying()) {
    return;
  }

}

// -----------------------------------------------------------------------------
void PianoRoll::setProject(ProjectContext *context) {
  mProject = context->project();
  mContext = context;
  update();

  connect(mContext, &ProjectContext::activeChannelChanged, this, &PianoRoll::setChannel);
  connect(mContext, &ProjectContext::codeChanged, this, &PianoRoll::onCodeChanged);
  connect(mContext, &ProjectContext::editorScrollChanged, this, &PianoRoll::onEditorScrollChanged);
  mNoteEditor->setProject(context);
  mPiano->setProject(context);
  setChannel(mContext->activeChannel());
}

// -----------------------------------------------------------------------------
void PianoRoll::setChannel(Sft::Channel *channel) {
  mSequenceHeader->setChannel(channel);
  mNoteEditor->setChannel(channel);
  update();
}

// -----------------------------------------------------------------------------
void PianoRoll::onCodeChanged() {
  update();
}

// -----------------------------------------------------------------------------
QSize PianoRoll::scrollableSize() const {
  int left = mConfiguration->pianoWidth();
  int right = 0;

  if (mContext->activeChannel()) {
    auto *executed = mContext->activeChannel()->script()->executed();
    
    if (executed) {
      right = executed->duration() * mConfiguration->measureTickWidth() / Sft::MEASURE_TICK_SUBDIVISION;
    }
  }

  return QSize(left + right + 1000, totalHeight() + mConfiguration->pianoHeaderHeight());
}

// -----------------------------------------------------------------------------
void PianoRoll::drawGL(QPainter *painter, QOpenGLFunctions *) {
  QSize area(size());

  RenderHelper h(painter, mConfiguration, area);
  h.clear();

  drawAllRenderables(h);
}

// -----------------------------------------------------------------------------
void PianoRoll::zoomEvent(float dir) {
  if (dir > 0) {
    if (mZoom > 1) {
      mZoom += 1;
    } else {
      mZoom *= 2;
    }
  } else {
    if (mZoom > 1) {
      mZoom -= 1;
    } else if (mZoom > 0.25) {
      mZoom /= 2;
    }
  }

  mNoteEditor->setZoom(mZoom);
  update();
}
