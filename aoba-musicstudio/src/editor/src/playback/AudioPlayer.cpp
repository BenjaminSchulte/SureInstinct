#include <QDebug>
#include <editor/playback/AudioPlayer.hpp>
#include <editor/playback/AudioManager.hpp>
#include <editor/playback/AudioSampleManager.hpp>
#include <sft/brr/BrrSample.hpp>
#include <sft/sample/Sample.hpp>
#include <cmath>

using namespace Editor;

AudioPlayer *AudioPlayer::mInstance = nullptr;

namespace {
  uint16_t noteValues[] = {
    0x260e, // C
    0x2851,
    0x2ab7, // D
    0x2d41,
    0x2ff2, // E
    0x32cc, // F
    0x35d2,
    0x3905, // G
    0x3c69,
    0x4000, // A
    0x43ce,
    0x47d7, // B
    0x4c1c
  };
}

// -----------------------------------------------------------------------------
AudioPlayer::AudioPlayer(AudioManager *manager)
  : mManager(manager)
{
  (void)mManager;
}

// -----------------------------------------------------------------------------
AudioPlayer::~AudioPlayer() {
}

// -----------------------------------------------------------------------------
bool AudioPlayer::assignInstrument(const Sft::InstrumentPtr &instrument, int channelIndex) {
  if (!instrument) {
    qWarning() << "Tried to use invalid instrument";
    return false;
  }

  AudioManager::Channel *channel = mManager->channel(channelIndex);
  int loopStart = instrument->sample()->loopStart() / instrument->sample()->maximumDuration() * instrument->sample()->brrSample()->numChunks();
  AudioSample sample(mManager->samples()->load(instrument->sample()->brrSample(), loopStart, instrument->isLooped()));
  if (!sample.isValid()) {
    return false;
  }

  channel->setAdsr(true, instrument->adsr().a, instrument->adsr().d, instrument->adsr().s, instrument->adsr().r);
  channel->setSample(sample.index());
  return true;
}

// -----------------------------------------------------------------------------
void AudioPlayer::play(const Sft::InstrumentPtr &instrument, int note, double volume, int channelIndex) {
  if (!instrument) {
    qWarning() << "Tried to use invalid instrument";
    return;
  }

  AudioManager::Channel *channel = mManager->channel(channelIndex);
  
  if (!assignInstrument(instrument, channelIndex)) {
    mManager->samples()->clear();
    assignInstrument(instrument, channelIndex);
  }

  channel->setPitch(calculatePitch(note, instrument->playbackNoteAdjust()));

  int dspVolume = volume * instrument->volume() * 127.0 + 0.5;
  channel->setVolume(dspVolume, dspVolume);
  channel->play();
}

// -----------------------------------------------------------------------------
uint16_t AudioPlayer::calculatePitch(double noteValue, double samplePitch, double channelPitch, double effectPitch) {
  double pitch = noteValue + samplePitch + channelPitch + effectPitch;
  double noteShift = std::fmod(pitch, 1.0);

  uint16_t octave = (uint16_t)pitch / 0xC;
  uint16_t note = (uint16_t)pitch % 0x0C;

  uint16_t notePitch = noteValues[note];
  uint16_t diff = ((double)noteValues[note + 1] - notePitch) * noteShift;

  uint16_t result = (notePitch + diff) >> (7 - octave);
  if (result >= 0x4000) {
    result = 0x3FFF;
  }
  //qDebug() << noteValue << samplePitch << channelPitch << effectPitch << "=" << QString::number(result, 16);
  return result;
}

// -----------------------------------------------------------------------------
AudioPlayer *AudioPlayer::instance() {
  if (!mInstance) {
    mInstance = new AudioPlayer(AudioManager::instance());
  }

  return mInstance;
}

// -----------------------------------------------------------------------------
void AudioPlayer::destroyInstance() {
  delete mInstance;
  mInstance = nullptr;
}