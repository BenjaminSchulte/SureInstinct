#pragma once

#include <QString>

namespace Sft {

class SampleDataReadInterface {
public:
  //! Constructor
  SampleDataReadInterface() = default;

  //! Deconstructor
  virtual ~SampleDataReadInterface() = default;

  //! Returns the number of channels
  virtual uint8_t numChannels() const = 0;

  //! Returns the number of samples per channel
  virtual uint32_t samplesPerChannel() const = 0;

  //! Returns the sample rate
  virtual uint32_t sampleRate() const = 0;

  //! Returns the duration
  inline double duration() const { return (double)samplesPerChannel() / (double)sampleRate(); }

  //! Returns a channel for reading
  virtual const double* channel(uint8_t index) const = 0;
  
  //! Returns a sample for reading
  virtual double sample(uint8_t channel, int index) const = 0;

};

}