#pragma once

#include <QDir>

namespace Sft {
class Song;
class Sample;

class SampleWriter {
public:
  //! Constructor
  SampleWriter(Sample *sample) : mSample(sample) {}

  //! Deconstructor
  virtual ~SampleWriter() = default;

  //! Returns the sample
  inline Sample *sample() const { return mSample; }

  //! Loads the sample
  virtual bool save(const QString &filename) const = 0;

protected:
  //! The sample to load
  Sample *mSample;
};

}