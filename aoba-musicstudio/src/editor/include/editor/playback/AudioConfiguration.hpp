#pragma once

namespace Editor {

class AudioConfiguration {
public:
  //! Constructor
  AudioConfiguration() = default;

  //! Deconstructor
  ~AudioConfiguration() = default;

  //! Returns the volume
  inline unsigned int volume() const { return mVolume; }

  //! Returns the latency
  inline unsigned int latency() const { return mLatency; }

  //! Returns the output frequency
  inline unsigned int outputFrequency() const { return mOutputFrequceny; }

protected:
  // The Volume
  unsigned int mVolume = 100;

  // The Latency
  unsigned int mLatency = 120;

  // The OutputFrequceny
  unsigned int mOutputFrequceny = 32000;
};

}