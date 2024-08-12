#pragma once

#include <QDir>

namespace Sft {
class Song;
class Sample;

class SampleLoader {
public:
  //! Constructor
  SampleLoader(Sample *sample) : mSample(sample) {}

  //! Deconstructor
  virtual ~SampleLoader() = default;

  //! Returns the sample
  inline Sample *sample() const { return mSample; }

  //! Loads the sample
  virtual bool load(const QString &filename) const = 0;

  //! Loads an external instrument
  static Sample *loadExternalSample(Song *song, const QString &id, const QDir &path);

protected:
  //! The sample to load
  Sample *mSample;
};

}