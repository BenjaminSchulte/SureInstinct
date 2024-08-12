#pragma once

#include <sft/instrument/Instrument.hpp>

namespace Editor {
class AudioManager;

class AudioPlayer {
public:
  //! Returns the instance
  static AudioPlayer *instance();

  //! Destroys the instance
  static void destroyInstance();

  //! Plays a note
  void play(const Sft::InstrumentPtr &instrument, int node, double volume=1.0, int channel=7);

  //! Plays a note
  bool assignInstrument(const Sft::InstrumentPtr &instrument, int channel);

  //! Calculates a pitch
  static uint16_t calculatePitch(double note, double samplePitch=0, double channelPitch=0, double effectPitch=0);

private:
  //! The constructor
  AudioPlayer(AudioManager *manager);

  //! The deconstructor
  ~AudioPlayer();

  //! The audio manager
  AudioManager *mManager;

  //! The instance
  static AudioPlayer *mInstance;
};

}