#include <QDebug>
#include <QImage>
#include <QPainter>
#include <QPainterPath>
#include <QGuiApplication>
#include <editor/sample/InstrumentWaveform.hpp>
#include <sft/instrument/Instrument.hpp>
#include <sft/sample/Sample.hpp>
#include <sft/sample/SampleData.hpp>

using namespace Editor;

// -----------------------------------------------------------------------------
InstrumentWaveform::InstrumentWaveform(QWidget *parent)
  : QWidget(parent)
{
}

// -----------------------------------------------------------------------------
InstrumentWaveform::~InstrumentWaveform() {
  deleteCachedImages();
}

// -----------------------------------------------------------------------------
void InstrumentWaveform::deleteCachedImages() {
  delete mOriginalSampleWaveform;

  mOriginalSampleWaveform = nullptr;
}

// -----------------------------------------------------------------------------
void InstrumentWaveform::setInstrument(const Sft::InstrumentPtr &inst) {
  mInstrument = inst;
  setSample(inst->sample());
}

// -----------------------------------------------------------------------------
void InstrumentWaveform::setSample(Sft::Sample *sample) {
  mSample = sample;
  mCacheIsValid = false;
  update();
}

// -----------------------------------------------------------------------------
void InstrumentWaveform::resizeEvent(QResizeEvent *event) {
  mCacheIsValid = false;

  QWidget::resizeEvent(event);
}

// -----------------------------------------------------------------------------
void InstrumentWaveform::paintEvent(QPaintEvent *) {
  if (!mCacheIsValid) {
    regenerateCache();
  }

  QPainter painter(this);
  painter.fillRect(QRect(QPoint(0, 0), size()), QGuiApplication::palette().color(QPalette::Base));
  painter.setPen(QGuiApplication::palette().color(QPalette::Window));
  painter.drawLine(0, center(), size().width(), center());
  painter.drawImage(QPoint(0, 0), *mOriginalSampleWaveform);

  if (mSample && mInstrument) {
    uint32_t width = size().width();
    const auto &adsr = mInstrument->adsr();
    
    double pixelPerSecond = (double)width / (mSample->previewData()->samplesPerChannel() / (double)mSample->previewData()->sampleRate());

    QPainterPath path;
    path.moveTo(0, center());
    path.lineTo(adsr.attackDuration() * pixelPerSecond, center() - waveHeight());
    path.lineTo((adsr.attackDuration() + adsr.decayDuration()) * pixelPerSecond, center() - waveHeight() * adsr.sustainPercent());

    double releaseDuration = adsr.releaseDuration();
    if (releaseDuration == std::numeric_limits<double>::infinity()) {
      releaseDuration = 1000000;
    }
    path.lineTo((adsr.attackDuration() + adsr.decayDuration() + releaseDuration) * pixelPerSecond, center());
    painter.setPen(QPen(QColor(0, 255, 255)));
    painter.drawPath(path);

    double loopPixelPerSecond = (double)width / mSample->maximumDuration();
    int loopLeft = mInstrument->sample()->loopStart() * loopPixelPerSecond;
    painter.setPen(QPen(QColor(255, 255, 0)));
    painter.drawLine(loopLeft, 0, loopLeft, size().height());
  }
}

// -----------------------------------------------------------------------------
void InstrumentWaveform::regenerateCache() {
  deleteCachedImages();

  mOriginalSampleWaveform = new QImage(size(), QImage::Format_ARGB32);
  mOriginalSampleWaveform->fill(QColor(0, 0, 0, 0));
  
  if (mSample) {
    drawWaveform(mOriginalSampleWaveform, mSample->inputData(), QColor(255, 0, 0, 255));
    drawWaveform(mOriginalSampleWaveform, mSample->previewData(), QColor(0, 255, 0, 128));
  }
}

// -----------------------------------------------------------------------------
void InstrumentWaveform::drawWaveform(QImage *target, const Sft::SampleData *data, const QColor &color) {
  uint32_t numSamples = data->samplesPerChannel();
  uint32_t width = size().width();
  double pixelPerSample = (double)width / numSamples;

  QPainter painter(target);
  painter.setPen(QPen(color));

  QPainterPath curve;
  double x = 0;
  int my = center();
  double wh = waveHeight();
  for (uint32_t i=0; i<numSamples; i++) {
    double y = my + data->sample(0, i) * wh;

    if (i == 0) {
      curve.moveTo(QPoint(x, y));
    } else {
      curve.lineTo(QPoint(x, y));
    }
    x += pixelPerSample;
  }

  painter.drawPath(curve);
}
