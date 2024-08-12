#pragma once

#include <QObject>
#include <sft/Configuration.hpp>
#include <libdsp/dsp.h>

class QTimer;
class QElapsedTimer;

namespace SNES {
class Dsp;
}

namespace Editor {
class AudioRam;
class AudioSampleManager;
class AudioSongProcessor;
class AudioConfiguration;
struct AudioRamBlock;

const uint32_t DSP_CLOCKS_PER_SECOND = 1024000;

// Timer runs at 8000Hz
const uint32_t DSP_TIMER_TICKS_PER_CLOCK = 128;

class AudioManager : public QObject {
  Q_OBJECT

public:
  struct Channel {  
    //! Constructor
    Channel(AudioManager *manager, uint8_t index)
      : mManager(manager), mChannel(index) {}

    //! Returns the index
    inline uint8_t index() const { return mChannel; }

    //! Initializes the channel
    void reset();

    //! Plays the note
    void play();

    //! Stops the note
    void stop();

    //! Sets the channel
    void setVolume(int left, int right);

    //! Sets the channel pitch
    void setPitch(uint16_t pitch);

    //! Sets the channel sample id
    void setSample(uint8_t sample);

    //! Sets the channel sample id
    void setAdsr(bool enable, uint8_t a, uint8_t d, uint8_t s, uint8_t r);

    //! Sets the channel sample id
    void setDirectGain(uint8_t parameter);

  protected:
    //! The audio manager
    AudioManager *mManager;

    //! The index
    uint8_t mChannel;
  };

  //! Returns the instance
  static AudioManager *instance();

  //! Destroys the instance
  static void deleteInstance();

  //! Returns the DSP
  inline SNES::Dsp *dsp() { return mDSP; }

  //! Returns the RAM
  inline AudioRam *ram() const { return mRAM; }

  //! Returns the samples
  inline AudioSampleManager *samples() const { return mSamples; }

  //! Returns a channel
  inline Channel *channel(uint8_t index) { return mChannels[index]; }

  //! Sets the master volume
  void setMasterVolume(int8_t left, int8_t right);

  //! Sets the echo volume
  void setEchoVolume(uint8_t left, uint8_t right);

  //! Sets the echo volume
  void setEchoFeedback(uint8_t feedback);

  //! Sets the echo volume
  void setEchoDelay(uint8_t delay);

  //! Sets the flag
  void setFlags(bool mute, bool echo, bool reset, uint8_t noiseClock);

  //! Sets the flag
  void setNoiseClock(uint8_t noiseClock);

  //! Sets the sample directory offset
  void setSampleDirectoryOffset(uint16_t offset);

  //! Sets the echo buffer offset
  void setEchoBufferOffset(uint16_t offset);

  //! Sets the timer
  void setSongPlaybackTimer(uint8_t timer);

  //! Sets key on
  void channelOn(uint8_t channel);

  //! Sets key off
  void channelOff(uint8_t channel);

  //! Sets key on
  void echoOn(uint8_t channel);

  //! Sets key off
  void echoOff(uint8_t channel);

  //! Sets key on
  void noiseOn(uint8_t channel);

  //! Sets key off
  void noiseOff(uint8_t channel);

  //! Sets key on
  void pitchModulationOn(uint8_t channel);

  //! Sets key off
  void pitchModulationOff(uint8_t channel);

  //! Returns the song processor
  inline AudioSongProcessor *songProcessor() { return mSongProcessor; }

  //! After DSP updated
signals:
  void onTimer();

private slots:
  //! Updates the DSP
  void processDsp();

private:
  //! Constructor
  explicit AudioManager();

  //! Deconstructor
  ~AudioManager();

  //! Initialize the DSP
  void initializeDSP();

  //! The instance
  static class AudioManager* mInstance;

  //! The audio configuration
  AudioConfiguration *mConfig;

  //! The song processor
  AudioSongProcessor *mSongProcessor;

  //! Whether everything has been initialized
  bool mInitialized;

  //! Whether the manager is started
  bool mStarted;

  //! The samples
  AudioSampleManager *mSamples;

  //! The block containing the echo buffer
  AudioRamBlock *mEchoBuffer;

  //! The DSP
  SNES::Dsp *mDSP;

  //! The RAM
  AudioRam *mRAM;

  //! The current key on value
  uint8_t mCurrentKeyOn = 0;

  //! The current key off value
  uint8_t mCurrentKeyOff = 0;

  //! The current echo on value
  uint8_t mEchoOn = 0;

  //! The current echo on value
  uint8_t mNoiseOn = 0;

  //! The current echo on value
  uint8_t mPitchModulationOn = 0;

  //! The current flags
  uint8_t mFlags = 0;

  //! The current timer 
  uint8_t mTimer = 0xFF;

  //! The current timer value (4 ticks per 1 DSP tick)
  int mTimerCounter = 0xFF * DSP_TIMER_TICKS_PER_CLOCK;

  //! List of all channels
  Channel *mChannels[Sft::MAX_NUM_CHANNEL];

  //! The timer used for updating the audio
  QTimer *mAudioTimer;

  //! The timer
  QElapsedTimer *mAudioTime;

  //! The last elapsed time
  uint64_t mLastTime = 0;
  
  //! The audio buffer
  SNES::Dsp::sample_t *mBuffer = nullptr;

  //! The audio buffer size in samples
  uint32_t mBufferSizeInSamples = 0;

  //! The current microseconds elapsed
  uint32_t mCurrentElapsed = 0;
};

}
