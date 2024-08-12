#include <QDebug>
#include <editor/playback/AudioSampleManager.hpp>
#include <editor/playback/AudioManager.hpp>
#include <editor/playback/AudioRam.hpp>
#include <sft/brr/BrrSample.hpp>
#include <sft/brr/BrrSampleChunk.hpp>
#include <sft/Configuration.hpp>

using namespace Editor;

// -----------------------------------------------------------------------------
AudioSampleManager::AudioSampleManager(AudioManager *manager)
  : mManager(manager)
{
  mSampleIndex = mManager->ram()->allocate(AudioRamBlock::SAMPLE_INDEX, Sft::MAX_NUM_SAMPLES * 4);
  mSampleIndexPointer = (uint16_t*)mManager->ram()->ram(mSampleIndex->offset());

  createNonLoopSample();
}

// -----------------------------------------------------------------------------
void AudioSampleManager::init() {
  mManager->setSampleDirectoryOffset(mSampleIndex->offset());
}

// -----------------------------------------------------------------------------
void AudioSampleManager::createNonLoopSample() {
  mNonLoopSample = mManager->ram()->allocate(AudioRamBlock::SAMPLE, Sft::NUM_BYTES_PER_BRR_SAMPLE_CHUNK);
  
  Sft::BrrSampleChunk chunk;
  for (uint8_t i=0; i<Sft::NUM_SAMPLES_PER_CHUNK; i++) {
    chunk.wave()[i] = 0;
  }

  chunk.setFilter(Sft::BrrSampleChunk::FILTER_1);
  chunk.writeTo(mManager->ram()->ram(mNonLoopSample->offset()), true, true);
}

// -----------------------------------------------------------------------------
void AudioSampleManager::clear() {
  auto copy(mSamples);

  for (const AudioSample &sample : copy) {
    unload(sample);
  }

  mSamples.clear();
}

// -----------------------------------------------------------------------------
AudioSample AudioSampleManager::load(Sft::BrrSample *sample, int loopStart, bool looped) {
  if (mSamples.contains(sample)) {
    updateSample(mSamples[sample], loopStart, looped);
    return mSamples[sample];
  }

  int32_t index = findFreeSampleId();
  if (index == -1) {
    qWarning() << "Maximum of loadable samples has been reached";
    return AudioSample();
  }

  AudioRamBlock *block = mManager->ram()->allocate(AudioRamBlock::SAMPLE, sample->streamLengthInBytes());
  if (!block) {
    qWarning() << "Out of memory in DSP RAM";
    return AudioSample();
  }

  AudioSample audioSample(index, block, sample);
  mSampleRegister[index] = audioSample;
  mSamples.insert(sample, audioSample);

  updateSample(audioSample, loopStart, looped);

  qDebug() << "Loaded sample to index" << index;
  sample->writeTo(mManager->ram()->ram(audioSample.block()->offset()));
  return audioSample;
}

// -----------------------------------------------------------------------------
void AudioSampleManager::updateSample(const AudioSample &sample, int loopStart, bool looped) {
  mSampleIndexPointer[sample.index() * 2] = sample.block()->offset();
  mSampleIndexPointer[sample.index() * 2 + 1] = looped ? (sample.block()->offset() + loopStart * 9) : mNonLoopSample->offset();
}

// -----------------------------------------------------------------------------
void AudioSampleManager::unload(const AudioSample &sample) {
  if (!sample.isValid()) {
    return;
  }

  if (mSampleRegister[sample.index()].sample() != sample.sample()) {
    return;
  }

  mManager->ram()->freeSample(sample.block());
  mSampleRegister[sample.index()] = AudioSample();
  mSamples.remove(sample.sample());
}

// -----------------------------------------------------------------------------
int32_t AudioSampleManager::findFreeSampleId() const {
  for (uint32_t i=0; i<Sft::MAX_NUM_SAMPLES; i++) {
    if (!mSampleRegister[i].isValid()) {
      return i;
    }
  }

  return -1;
}
