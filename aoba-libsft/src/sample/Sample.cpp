#include <QDebug>
#include <QFileInfo>
#include <cmath>
#include <sft/sample/SampleData.hpp>
#include <sft/sample/Sample.hpp>
#include <sft/sample/WavSampleLoader.hpp>
#include <sft/sample/BrrSampleLoader.hpp>
#include <sft/brr/BrrSample.hpp>
#include <sft/brr/BrrCodec.hpp>
#include <sft/Configuration.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
Sample::Sample(const QString &id, const QString &name)
  : mId(id)
  , mName(name)
  , mInputData(new SampleData())
{
}

// -----------------------------------------------------------------------------
Sample::~Sample() {
  delete mBrrSample;
  delete mInputData;
  delete mPreviewData;
}

// -----------------------------------------------------------------------------
bool Sample::load(const QString &fileName) {
  QFileInfo info(fileName);
  QString suffix = info.suffix().toLower();

  if (suffix == "wav") {
    WavSampleLoader loader(this);
    return loader.load(fileName);
  } else if (suffix == "brr") {
    BrrSampleLoader loader(this);
    return loader.load(fileName);
  } else {
    qWarning() << "Unsupported sample format:" << suffix;
  }
  
  return false;
}

// -----------------------------------------------------------------------------
double Sample::maximumDuration() const {
  if (mIsRawBrrSample) {
    return mBrrSample->numChunks() * 16 / (double)DSP_FREQUENCY;
  }

  if (mInputData) {
    return mInputData->duration();
  }

  return 0;
}

// -----------------------------------------------------------------------------
BrrSample *Sample::createRawBrrSample() {
  if (mBrrSample) {
    delete mBrrSample;
  }

  mIsRawBrrSample = true;
  mBrrSample = new BrrSample();
  return mBrrSample;
}

// -----------------------------------------------------------------------------
const SampleData *Sample::outputData() const {
  return mInputData;
}

// -----------------------------------------------------------------------------
void Sample::setCompressionFactor(double factor) {
  mCompressionFactor = factor;

  if (!mIsRawBrrSample) {
    delete mPreviewData;
    delete mBrrSample;
    mPreviewData = nullptr;
    mBrrSample = nullptr;
  }
}

// -----------------------------------------------------------------------------
const SampleData *Sample::previewData() const {
  if (mPreviewData) {
    return mPreviewData;
  }
  
  mPreviewData = new SampleData();

  BrrCodec codec;
  codec.loadFromBrr(brrSample());
  codec.decode();
  codec.toWave(mPreviewData);

  return mPreviewData;
}

// -----------------------------------------------------------------------------
double Sample::playbackNoteAdjust() const {
  if (mIsRawBrrSample) {
    return 0;
  }

  double divided = ((double)outputData()->sampleRate() * mCompressionFactor) / (double)DSP_FREQUENCY;
  double reduced = std::log(divided) / std::log(2);
  double multiplicated = reduced * Sft::NUM_NOTES_PER_OCTAVE;

  return multiplicated;
}

// -----------------------------------------------------------------------------
BrrSample *Sample::brrSample() const {
  if (mBrrSample) {
    return mBrrSample;
  }

  BrrCodec codec;
  double loopOffset = -1;
  if (mLooped) {
    loopOffset = mLoopStart;
  }
  
  if (mCompressionFactor == 1.0) {
    codec.loadFromWave(outputData(), loopOffset);
  } else {
    SampleData copy(*outputData());
    copy.resample((double)copy.sampleRate() * mCompressionFactor);
    codec.loadFromWave(&copy, loopOffset);
  }

  mBrrSample = new BrrSample();
  codec.encode();
  codec.toBrrSample(mBrrSample);
  mBrrSample->setLoopEnabled(mLooped);

  return mBrrSample;
}

// -----------------------------------------------------------------------------
double Sample::loopStart() const {
  return mLoopStart / (double)brrSample()->numChunks() * maximumDuration();
}

// -----------------------------------------------------------------------------
void Sample::setLoopStart(double loopStart) {
  mLoopStart = loopStart / maximumDuration() * brrSample()->numChunks();
}

// -----------------------------------------------------------------------------
uint32_t Sample::loopStartChunk() const {
  return mLoopStart;
}

// -----------------------------------------------------------------------------
void Sample::setLoopStartChunk(uint32_t loopStart) {
  mLoopStart = loopStart;

  if (!mIsRawBrrSample) {
    delete mBrrSample;
    mBrrSample = nullptr;
  }
}

// -----------------------------------------------------------------------------
void Sample::setLooped(bool looped) {
  mLooped = looped;

  if (!mIsRawBrrSample) {
    delete mBrrSample;
    mBrrSample = nullptr;
  }
}