#pragma once

#include <QString>
#include <vector>

namespace Sft {

class BrrSample;
class SampleData;

class BrrCodec {
public:
  BrrCodec();
  ~BrrCodec();

  void reset();

  void decode();
  void encode();

  void loadFromBrr(const BrrSample *sample, int loopFromChunk=-1);
  void loadFromWave(const SampleData *data, double loopFromOffset=-1);
  void toBrrSample(BrrSample *sample) const;
  void toWave(SampleData *sample, int sampleRate=32000) const;

  inline bool isLoopEnabled() const { return mLoopEnabled; }
  inline uint16_t loopStartSample() const { return mLoopStart; }

private:
  uint32_t mLoopStart;
  bool mLoopEnabled;
  bool mGaussEnabled;
  bool mUserPitchEnabled;
  bool mHeaderInclude;
  bool mOldLoop;

  uint16_t mPitchStepBase;

  uint32_t mInputSampleRate;
  uint32_t mOutputSampleRate;

  std::vector<int16_t> mWavData;
  std::vector<uint8_t> mBrrData;

  //void (FASTCALL *cb_func)(BrrCodec&);
  //void* cb_data;

  //uint cur_progress;
  //uint last_progress;
  //uint cb_progress;

  uint32_t mTotalBlocks;
  double mTotalError;
  double mAverageError;
  double mMinError;
  double mMaxError;
};

}
