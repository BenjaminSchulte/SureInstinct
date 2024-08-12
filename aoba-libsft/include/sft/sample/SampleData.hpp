#pragma once

#include "SampleDataReadInterface.hpp"

namespace Sft {

class SampleData : public SampleDataReadInterface {
public:
  //! Constructor
  SampleData() = default;

  //! Copy constructor
  SampleData(const SampleData &other);

  //! Deconstructor
  ~SampleData();

  //! Clears the sample
  void clear();

  //! Allocates the sample data
  void allocate(uint8_t channels, uint32_t sampleRate, uint32_t samplesPerChannel);

  //! Returns the number of channels
  uint8_t numChannels() const override { return mNumChannels; }

  //! Returns the number of samples per channel
  uint32_t samplesPerChannel() const override { return mSamplesPerChannel; }

  //! Returns the sample rate
  uint32_t sampleRate() const override { return mSampleRate; }

  //! Returns a channel for reading
  const double* channel(uint8_t index) const override { return mSamples[index]; }

  //! Returns a channel
  inline double* channel(uint8_t index) { return mSamples[index]; }

  //! Returns a sample
  inline double &sample(uint8_t channel, int index) {
    if (index < 0 || index >= (int)mSamplesPerChannel) { return mInvalidSample; }
    return mSamples[channel][index];
  }

  //! Returns a sample
  double sample(uint8_t channel, int index) const override {
    if (index < 0 || index >= (int)mSamplesPerChannel) { return 0; }
    return mSamples[channel][index];
  }

  //! Resamples the data
  void resample(uint32_t targetSampleRate);

private:
  //! Resamples a single channel
  uint32_t resampleChannel(uint8_t channel, uint32_t inputRate, uint32_t outputRate);

  //! Converts an array from/to another type
  template<typename TIN, typename TOUT>
  static TOUT *convertStream(TIN *data, uint32_t size) {
    TOUT *result = new TOUT[size];
    for (uint32_t i=0; i<size; i++) {
      result[i] = data[i];
    }
    return result;
  }

  //! The number of channels
  uint8_t mNumChannels = 0;

  //! Pointer to the data
  double **mSamples = nullptr;

  //! The sample rate
  uint32_t mSampleRate;

  //! Number of samples per channel
  uint32_t mSamplesPerChannel;

  //! Invalid sample
  double mInvalidSample = 0;
};

}