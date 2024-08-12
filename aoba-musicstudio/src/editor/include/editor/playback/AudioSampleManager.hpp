#pragma once

#include <QMap>
#include <sft/Configuration.hpp>

namespace Sft {
class BrrSample;
}

namespace Editor {
class AudioManager;
struct AudioRamBlock;

struct AudioSample {
  //! Invalid Constructor
  AudioSample() : mIndex(-1) {}

  //! Constructor
  AudioSample(int32_t index, AudioRamBlock *block, Sft::BrrSample *sample)
    : mIndex(index), mBlock(block), mSample(sample) {}

  //! Returns whether this sample is valid
  inline bool isValid() const { return mIndex >= 0; }

  //! Returns the index
  inline int32_t index() const { return mIndex; }
  
  //! Returns the block
  inline AudioRamBlock *block() const { return mBlock; }

  //! Returns the sample
  inline Sft::BrrSample *sample() const { return mSample; }

protected:
  //! The index
  int32_t mIndex;

  //! The RAM block
  AudioRamBlock *mBlock;

  //! The sample
  Sft::BrrSample *mSample;
};

class AudioSampleManager {
public:
  //! Constructor
  AudioSampleManager(AudioManager *manager);

  //! Initialize
  void init();

  //! Clears the ram
  void clear();

  //! Allocates a sample
  AudioSample load(Sft::BrrSample *sample, int loopStart, bool looped);

  //! Unloads a sample
  void unload(const AudioSample &sample);

protected:
  //! Returns a free sample ID (-1 if no slot could be found)
  int32_t findFreeSampleId() const;

  //! Updates a sample
  void updateSample(const AudioSample &sample, int loopStart, bool looped);

  //! Create the empty-sample
  void createNonLoopSample();

  //! The audio manager
  AudioManager *mManager;

  //! The block containing the sample index
  AudioRamBlock *mSampleIndex;

  //! RAM block of non looped sample
  AudioRamBlock *mNonLoopSample;

  //! The sample index RAM pointer
  uint16_t *mSampleIndexPointer;

  //! A list of all samples
  QMap<Sft::BrrSample*, AudioSample> mSamples;
  
  //! A list of samples with the given IDs
  AudioSample mSampleRegister[Sft::MAX_NUM_SAMPLES];
};

}