#pragma once

#include "../instrument/InstrumentList.hpp"
#include "../sample/SampleList.hpp"
#include "../channel/Channel.hpp"

namespace Sft {

class Song {
public:
  //! Constructor
  Song();

  //! Deconstructor
  ~Song();

  //! Returns the samples
  inline SampleList &samples() { return mSamples; }

  //! Returns the instruments
  inline InstrumentList &instruments() { return mInstruments; }

  //! Returns the amount of channels
  inline uint8_t numChannels() const { return mNumChannels; }

  //! Returns a channel
  inline Channel *channel(uint8_t index) { return mChannels[index]; }

protected:
  //! Initializes all channels
  void initializeChannels();

  //! List of all samples
  SampleList mSamples;

  //! List of all instruments
  InstrumentList mInstruments;

  //! The amount of channels
  uint8_t mNumChannels;

  //! Pointer to all channels
  Channel **mChannels;
};

}