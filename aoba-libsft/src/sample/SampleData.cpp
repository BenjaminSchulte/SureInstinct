#include <QDebug>
#include <cmath>
#include <samplerate.h>
#include <sft/sample/SampleData.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
SampleData::SampleData(const SampleData &other)
  : mNumChannels(other.mNumChannels)
  , mSampleRate(other.mSampleRate)
  , mSamplesPerChannel(other.mSamplesPerChannel)
{
  mSamples = new double*[mNumChannels];
  for (uint8_t c=0; c<mNumChannels; c++) {
    mSamples[c] = new double[mSamplesPerChannel];

    memcpy(mSamples[c], other.mSamples[c], mSamplesPerChannel * sizeof(double));
  }
}

// -----------------------------------------------------------------------------
SampleData::~SampleData() {
  clear();
}

// -----------------------------------------------------------------------------
void SampleData::clear() {
  if (!mSamples) {
    return;
  }

  for (uint8_t c=0; c<mNumChannels; c++) {
    delete[] mSamples[c];
  }

  delete[] mSamples;
  mSamples = nullptr;
}

// -----------------------------------------------------------------------------
void SampleData::allocate(uint8_t channels, uint32_t sampleRate, uint32_t samplesPerChannel) {
  clear();

  mNumChannels = channels;
  mSampleRate = sampleRate;
  mSamplesPerChannel = samplesPerChannel;

  mSamples = new double*[channels];
  for (uint8_t c=0; c<mNumChannels; c++) {
    mSamples[c] = new double[samplesPerChannel];
  }
}

// -----------------------------------------------------------------------------
void SampleData::resample(uint32_t targetSampleRate) {
  uint32_t newNumSample = 0;
  for (uint8_t c=0; c<mNumChannels; c++) {
    newNumSample = resampleChannel(c, mSampleRate, targetSampleRate);
    if (newNumSample == 0) {
      return;
    }
  }

  mSamplesPerChannel = newNumSample;
  mSampleRate = targetSampleRate;
}

// -----------------------------------------------------------------------------
uint32_t SampleData::resampleChannel(uint8_t channel, uint32_t inputRate, uint32_t outputRate) {
  double ratio = (double)outputRate / (double)inputRate;
  uint32_t outputSize = std::ceil((double)mSamplesPerChannel * ratio);

  SRC_DATA src_data;
  src_data.data_in = convertStream<double, float>(mSamples[channel], mSamplesPerChannel);
  src_data.data_out = new float[outputSize];
  src_data.input_frames = mSamplesPerChannel;
  src_data.output_frames = outputSize;
  src_data.src_ratio = ratio;

  int result = src_simple(&src_data, SRC_SINC_MEDIUM_QUALITY, 1);
  if (result) {
    qWarning() << "Failed to convert sample rate" << src_strerror(result);
    return 0;
  }

  delete[] mSamples[channel];
  mSamples[channel] = convertStream<float, double>(src_data.data_out, outputSize);
  delete[] src_data.data_in;
  delete[] src_data.data_out;

  return src_data.output_frames_gen;
}
