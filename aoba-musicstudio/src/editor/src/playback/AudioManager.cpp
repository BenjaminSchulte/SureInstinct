#include <QDebug>
#include <QTimer>
#include <QElapsedTimer>
#include <ruby/ruby.hpp>
#include <libdsp/dsp.h>
#include <editor/playback/AudioConfiguration.hpp>
#include <editor/playback/AudioManager.hpp>
#include <editor/playback/AudioRam.hpp>
#include <editor/playback/AudioSampleManager.hpp>
#include <editor/playback/AudioSongProcessor.hpp>
#include <sft/Configuration.hpp>

using namespace Editor;

// ----------------------------------------------------------------------------
AudioManager *AudioManager::mInstance;

// ----------------------------------------------------------------------------
AudioManager::AudioManager()
  : mConfig(new AudioConfiguration())
  , mSongProcessor(new AudioSongProcessor(this))
  , mStarted(false)
  , mDSP(new SNES::Dsp())
  , mRAM(new AudioRam())
  , mAudioTimer(new QTimer(this))
  , mAudioTime(new QElapsedTimer())
{
  for (uint8_t i=0; i<Sft::MAX_NUM_CHANNEL; i++) {
    mChannels[i] = new Channel(this, i);
  }

  mEchoBuffer = mRAM->allocate(AudioRamBlock::ECHO_BUFFER, Sft::MAX_ECHO_BUFFER_SIZE);
  mSamples = new AudioSampleManager(this);

  mInitialized = ruby::audio.init();
  if (mInitialized) {
    initializeDSP();
  } else {
    qWarning() << "Failed to initialize audio driver.";

    ruby::audio.driver("None");
    ruby::audio.init();
  }
}

// ----------------------------------------------------------------------------
AudioManager::~AudioManager() {
  mAudioTimer->stop();
  delete[] mBuffer;
  delete mAudioTime;
  if (mInitialized) {
    ruby::audio.term();
  }
  for (uint8_t i=0; i<Sft::MAX_NUM_CHANNEL; i++) {
    delete mChannels[i];
  }
  delete mSamples;
  delete mDSP;
  delete mRAM;
  delete mConfig;
}

// ----------------------------------------------------------------------------
void AudioManager::initializeDSP() {
  unsigned int frequency = mConfig->outputFrequency();
  unsigned int inputFrequency = Sft::DSP_FREQUENCY;
  mBufferSizeInSamples = frequency;

  mBuffer = new SNES::Dsp::sample_t[mBufferSizeInSamples];
  mDSP->init(mRAM->ram());
  mDSP->set_output(mBuffer, mBufferSizeInSamples);
  mDSP->reset();

  mSamples->init();
  
  qDebug() << "Using audio driver:" << ruby::audio.default_driver();

  ruby::audio.set(ruby::Audio::Frequency, frequency);
  ruby::audio.set(ruby::Audio::Latency, mConfig->latency());
  ruby::audio.set(ruby::Audio::Volume, mConfig->volume());
  ruby::audio.set(ruby::Audio::Synchronize, false);
  ruby::audio.set(ruby::Audio::Resample, inputFrequency != frequency);
  ruby::audio.set(ruby::Audio::ResampleRatio, (double)inputFrequency / (double)frequency);

  setMasterVolume(0x7F, 0x7F);
  setEchoVolume(0xDC, 0xDC);
  setEchoFeedback(0x38);
  setEchoDelay(5);
  setEchoBufferOffset(mEchoBuffer->offset());
  setFlags(false, true, false, 0);
  
  mDSP->write(SNES::Dsp::r_non, 0);
  mDSP->write(SNES::Dsp::r_eon, 0);
  mDSP->write(SNES::Dsp::r_pmon, 0);
  
  mDSP->write(SNES::Dsp::r_fir | 0x00, 0x20);
  mDSP->write(SNES::Dsp::r_fir | 0x10, 0x0c);
  mDSP->write(SNES::Dsp::r_fir | 0x20, 0x10);
  mDSP->write(SNES::Dsp::r_fir | 0x30, 0x0c);
  mDSP->write(SNES::Dsp::r_fir | 0x40, 0x20);
  mDSP->write(SNES::Dsp::r_fir | 0x50, 0x0c);
  mDSP->write(SNES::Dsp::r_fir | 0x60, 0x0c);
  mDSP->write(SNES::Dsp::r_fir | 0x70, 0x0c);
  
  mCurrentKeyOff = 0xFF;
  channelOff(0);

  for (int i=0; i<8; i++) {
    mChannels[i]->reset();
  }

  connect(mAudioTimer, &QTimer::timeout, this, &AudioManager::processDsp);
  mAudioTimer->setInterval(10);
  mAudioTime->restart();
  mCurrentElapsed = mConfig->latency();
  mAudioTimer->start();
}

// ----------------------------------------------------------------------------
void AudioManager::setSongPlaybackTimer(uint8_t timer) {
  if (timer == 0) {
    timer = 0xFF;
  }

  mTimer = timer;
  mTimerCounter = timer * 4; // 4 ticks per 1 DSP tick
}

// ----------------------------------------------------------------------------
void AudioManager::processDsp() {

  uint64_t thisTotalTime = mAudioTime->elapsed();
  uint64_t thisTime = thisTotalTime - mLastTime;
  mLastTime = thisTotalTime;
  mCurrentElapsed += thisTime;

  uint32_t clocks = (double)mCurrentElapsed / 1000 * (double)DSP_CLOCKS_PER_SECOND;
  if (!clocks) {
    return;
  }

  mCurrentElapsed -= (double)clocks / (double)DSP_CLOCKS_PER_SECOND * 1000;

  while (clocks) {
    uint32_t thisClock = qMin<uint32_t>(clocks, mTimerCounter);
    mDSP->run(thisClock);
    clocks -= thisClock;

    mTimerCounter -= thisClock;
    if (mTimerCounter <= 0) {
      mSongProcessor->onTimer();
      mTimerCounter = mTimer * DSP_TIMER_TICKS_PER_CLOCK;

      mDSP->write(SNES::Dsp::r_koff, mCurrentKeyOff);
      mDSP->write(SNES::Dsp::r_kon, mCurrentKeyOn);
      mCurrentKeyOn = 0;
      //mCurrentKeyOff = 0;
    }

    int count = mDSP->sample_count() >> 1;

    SNES::Dsp::sample_t *sample = mBuffer;
    while (count--) {
      int l = sample[0];
      int r = sample[1];
      sample += 2;

      if (l < -32768 || l >= 32768) {
        qDebug() << l;
      }

      ruby::audio.sample(l, r);
    }

    mDSP->set_output(mBuffer, mBufferSizeInSamples);
  }

  emit onTimer();
}

// ----------------------------------------------------------------------------
void AudioManager::setMasterVolume(int8_t left, int8_t right) {
  mDSP->write(SNES::Dsp::r_mvoll, left);
  mDSP->write(SNES::Dsp::r_mvolr, right);
}

// ----------------------------------------------------------------------------
void AudioManager::setEchoVolume(uint8_t left, uint8_t right) {
  mDSP->write(SNES::Dsp::r_evoll, left);
  mDSP->write(SNES::Dsp::r_evolr, right);
}

// ----------------------------------------------------------------------------
void AudioManager::setEchoFeedback(uint8_t feedback) {
  mDSP->write(SNES::Dsp::r_efb, feedback);
}

// ----------------------------------------------------------------------------
void AudioManager::setEchoDelay(uint8_t delay) {
  mDSP->write(SNES::Dsp::r_edl, delay);
}

// ----------------------------------------------------------------------------
void AudioManager::setFlags(bool mute, bool echo, bool reset, uint8_t noiseClock) {
  mFlags = 
    (reset ? 0x80 : 0x00) |
    (mute  ? 0x40 : 0x00) |
    (echo  ? 0x00 : 0x20) |
    noiseClock;

  mDSP->write(SNES::Dsp::r_flg, mFlags);
}

// ----------------------------------------------------------------------------
void AudioManager::setNoiseClock(uint8_t noiseClock) {
  mFlags = (mFlags & 0xE0) | (noiseClock & 0x1F);
  mDSP->write(SNES::Dsp::r_flg, mFlags);
}

// ----------------------------------------------------------------------------
void AudioManager::setSampleDirectoryOffset(uint16_t offset) {
  if ((offset & 0xFF) != 0) {
    qWarning() << "Sample directory offset must be multiplier of 0x100";
    return;
  } 

  mDSP->write(SNES::Dsp::r_dir, offset >> 8);
}

// ----------------------------------------------------------------------------
void AudioManager::setEchoBufferOffset(uint16_t offset) {
  if ((offset & 0xFF) != 0) {
    qWarning() << "Echo buffer offset must be multiplier of 0x100";
    return;
  } 

  mDSP->write(SNES::Dsp::r_esa, offset >> 8);
}

// ----------------------------------------------------------------------------
void AudioManager::Channel::play() {
  mManager->channelOn(mChannel);
}

// ----------------------------------------------------------------------------
void AudioManager::Channel::stop() {
  mManager->channelOff(mChannel);
}

// ----------------------------------------------------------------------------
void AudioManager::Channel::reset() {
  setVolume(127, 127);
  setPitch(0x1000);
  setSample(0);
  setAdsr(false, 0, 0, 0, 0);
  setDirectGain(0);
} 

// ----------------------------------------------------------------------------
void AudioManager::Channel::setDirectGain(uint8_t parameter) {
  mManager->dsp()->write(SNES::Dsp::v_gain | (mChannel * 0x10), parameter & 0x7F);
}

// ----------------------------------------------------------------------------
void AudioManager::Channel::setVolume(int left, int right) {
  mManager->dsp()->write(SNES::Dsp::v_voll | (mChannel * 0x10), left);
  mManager->dsp()->write(SNES::Dsp::v_volr | (mChannel * 0x10), right);
}

// ----------------------------------------------------------------------------
void AudioManager::Channel::setPitch(uint16_t pitch) {
  mManager->dsp()->write(SNES::Dsp::v_pitchl | (mChannel * 0x10), pitch);
  mManager->dsp()->write(SNES::Dsp::v_pitchh | (mChannel * 0x10), pitch >> 8);
}

// ----------------------------------------------------------------------------
void AudioManager::Channel::setSample(uint8_t sample) {
  mManager->dsp()->write(SNES::Dsp::v_srcn | (mChannel * 0x10), sample);
}

// ----------------------------------------------------------------------------
void AudioManager::Channel::setAdsr(bool enable, uint8_t a, uint8_t d, uint8_t s, uint8_t r) {
  mManager->dsp()->write(SNES::Dsp::v_adsr0 | (mChannel * 0x10), 
    (enable ? 0x80 : 0) |
    ((d & 0x7) << 4) |
    (a & 0xF)
  );
  mManager->dsp()->write(SNES::Dsp::v_adsr1 | (mChannel * 0x10), 
    ((s & 0x7) << 5) |
    (r & 0x1F)
  );
}

// ----------------------------------------------------------------------------
void AudioManager::channelOn(uint8_t channel) {
  mCurrentKeyOn |= 1 << channel;
  mCurrentKeyOff &= ~(1 << channel);
}

// ----------------------------------------------------------------------------
void AudioManager::channelOff(uint8_t channel) {
  mCurrentKeyOff |= 1 << channel;
  mCurrentKeyOn &= ~(1 << channel);
}

// ----------------------------------------------------------------------------
void AudioManager::echoOn(uint8_t echo) {
  mEchoOn |= 1 << echo;
  mDSP->write(SNES::Dsp::r_eon, mEchoOn);
}

// ----------------------------------------------------------------------------
void AudioManager::echoOff(uint8_t echo) {
  mEchoOn &= ~(1 << echo);
  mDSP->write(SNES::Dsp::r_eon, mEchoOn);
}

// ----------------------------------------------------------------------------
void AudioManager::noiseOn(uint8_t noise) {
  mNoiseOn |= 1 << noise;
  mDSP->write(SNES::Dsp::r_non, mNoiseOn);
}

// ----------------------------------------------------------------------------
void AudioManager::noiseOff(uint8_t noise) {
  mNoiseOn &= ~(1 << noise);
  mDSP->write(SNES::Dsp::r_non, mNoiseOn);
}

// ----------------------------------------------------------------------------
void AudioManager::pitchModulationOn(uint8_t pitchModulation) {
  mPitchModulationOn |= 1 << pitchModulation;
  mDSP->write(SNES::Dsp::r_pmon, mPitchModulationOn);
}

// ----------------------------------------------------------------------------
void AudioManager::pitchModulationOff(uint8_t pitchModulation) {
  mPitchModulationOn &= ~(1 << pitchModulation);
  mDSP->write(SNES::Dsp::r_pmon, mPitchModulationOn);
}

// ----------------------------------------------------------------------------
AudioManager *AudioManager::instance() {
  if (!mInstance) {
    mInstance = new AudioManager;
  }

  return mInstance;
}

// ----------------------------------------------------------------------------
void AudioManager::deleteInstance() {
  delete mInstance;
  mInstance = nullptr;
}