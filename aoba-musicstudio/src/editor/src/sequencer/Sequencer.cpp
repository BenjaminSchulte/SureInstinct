#include <QDebug>
#include <editor/application/DisplayConfiguration.hpp>
#include <editor/sequencer/Sequencer.hpp>
#include <editor/sequencer/SequencerChannel.hpp>
#include <editor/render/RenderHelper.hpp>
#include <editor/project/Project.hpp>
#include <editor/project/ProjectContext.hpp>
#include <editor/playback/AudioManager.hpp>
#include <editor/playback/AudioSongProcessor.hpp>
#include <sft/song/Song.hpp>
#include <sft/mml/MmlScript.hpp>
#include <sft/mml/MmlExecutedScript.hpp>

using namespace Editor;

// -----------------------------------------------------------------------------
Sequencer::Sequencer(QWidget *parent)
  : RenderScrollableWidget(parent)
  , mProject(nullptr)
  , mContext(nullptr)
{
  setMinimumSize(QSize(200, 8 * 13));

  connect(AudioManager::instance(), &AudioManager::onTimer, this, &Sequencer::onAudioTimer);
}

// -----------------------------------------------------------------------------
void Sequencer::onAudioTimer() {
  AudioSongProcessor *processor = AudioManager::instance()->songProcessor();
  if (!processor->isPlaying()) {
    return;
  }

  update();
}

// -----------------------------------------------------------------------------
void Sequencer::redraw() {
  update();
}

// -----------------------------------------------------------------------------
void Sequencer::recreateContent() {
  removeAllRenderables();
  if (!mContext) {
    return;
  }

  mChannels.clear();

  QSize area(size());
  double heightPerChannel = area.height() / (double)mProject->song()->numChannels();
  if (heightPerChannel < 2) {
    return;
  }

  double y = 0;
  for (int i=0; i<mProject->song()->numChannels(); i++) {
    SequencerChannel *channel = new SequencerChannel(this, QRect(0, y, area.width(), heightPerChannel), mContext, i);
    channel->setZoom(mZoom);
    mRenderables.push_back(channel);
    mChannels.push_back(channel);
    y += heightPerChannel;
  }
}

// -----------------------------------------------------------------------------
void Sequencer::doScroll(int, int) {
  for (auto *channel : mChannels) {
    channel->setScroll(mScrollPosition.x());
  }
  update();
}

// -----------------------------------------------------------------------------
bool Sequencer::isRenderable() const {
  return RenderScrollableWidget::isRenderable() && !!mProject;
}

// -----------------------------------------------------------------------------
void Sequencer::setProject(ProjectContext *context) {
  removeAllRenderables();

  mProject = context->project();
  mContext = context;
  
  connect(mContext, &ProjectContext::activeChannelChanged, this, &Sequencer::setChannel);
  connect(mContext, &ProjectContext::codeChanged, this, &Sequencer::onCodeChanged);
  connect(mContext, &ProjectContext::editorScrollChanged, this, &Sequencer::redraw);
  connect(mContext, &ProjectContext::editorWidthChanged, this, &Sequencer::redraw);
  
  recreateContent();
  update();
}

// -----------------------------------------------------------------------------
void Sequencer::onCodeChanged() {
  update();
}

// -----------------------------------------------------------------------------
void Sequencer::setChannel(Sft::Channel *) {
  update();
}

// -----------------------------------------------------------------------------
QSize Sequencer::scrollableSize() const {
  int width = 0;

  if (mProject) {
    for (int i=0; i<mProject->song()->numChannels(); i++) {
      auto *script = mProject->song()->channel(i)->script();
      script->requireCompiled();
      width = qMax<int>(width, script->executed()->duration() * mZoom);
    }
  }

  return QSize(width + 100, 8);
}

// -----------------------------------------------------------------------------
void Sequencer::drawGL(QPainter *painter, QOpenGLFunctions *) {
  QSize area(size());

  RenderHelper h(painter, mConfiguration, area);
  h.clear();

  drawAllRenderables(h);
}

// -----------------------------------------------------------------------------
void Sequencer::wheelEvent(QWheelEvent *event) {
  float dir = event->angleDelta().y() / 120.0;

  if (dir > 0) {
    if (mZoom > 1) {
      mZoom += 1;
    } else {
      mZoom *= 2;
    }
  } else {
    if (mZoom > 1) {
      mZoom -= 1;
    } else if (mZoom > 0.125) {
      mZoom /= 2;
    }
  }

  for (auto *channel : mChannels) {
    channel->setZoom(mZoom);
  }

  update();
}
