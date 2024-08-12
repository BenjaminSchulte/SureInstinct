#pragma once

#include <QString>

namespace Sft {
class SampleData;
class BrrSample;

class Sample {
public:
  //! Constructor
  Sample(const QString &id, const QString &name);

  //! Deconstructor
  ~Sample();

  //! Sets the ID
  void setId(const QString &id) { mId = id; }

  //! Returns the name
  inline const QString &id() const { return mId; }

  //! Returns the name
  inline const QString &name() const { return mName; }

  //! Returns the input data
  inline SampleData *inputData() { return mInputData; }

  //! Sets the sample to be a raw sample
  inline bool isRawBrrSample() const { return mIsRawBrrSample; }
 
  //! Sets the sample to be a raw sample
  BrrSample *createRawBrrSample();

  //! Returns the input data
  inline const SampleData *inputData() const { return mInputData; }

  //! Returns the compression factor
  inline double compressionFactor() const { return mCompressionFactor; }

  //! Sets the compression factor
  void setCompressionFactor(double factor);

  //! Returns the duration
  double maximumDuration() const;

  //! Returns the output duration
  double generatedDuration() const { return maximumDuration(); }

  //! Returns the generated output data
  const SampleData *outputData() const;

  //! Returns the generated output data
  const SampleData *previewData() const;

  //! The pitch multiplicator
  double playbackNoteAdjust() const;

  //! Loads the sample from a file
  bool load(const QString &fileName);

  //! Generates and returns the BRR sample
  BrrSample *brrSample() const;
  
  //! Returns the loop start
  double loopStart() const;

  //! Sets the loopStart
  void setLoopStart(double loopStart);
  
  //! Returns the loop start
  uint32_t loopStartChunk() const;

  //! Sets the loopStart
  void setLoopStartChunk(uint32_t loopStart);

  //! Returns the loop
  inline bool isLooped() const { return mLooped; }

  //! Sets the looped
  void setLooped(bool looped);

protected:
  //! The id
  QString mId;

  //! The name
  QString mName;

  //! The compression factor
  double mCompressionFactor = 1.0;

  //! The sample input data
  SampleData *mInputData = nullptr;

  //! The sample output data
  mutable SampleData *mPreviewData = nullptr;

  //! Whether the sample is a RAW BRR sample
  bool mIsRawBrrSample = false;

  //! The BRR sample
  mutable BrrSample *mBrrSample = nullptr;

  //! Loop start in samples
  uint32_t mLoopStart;

  //! Whether the instrument is looped
  bool mLooped = false;
};

}