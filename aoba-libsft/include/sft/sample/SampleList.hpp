#pragma once

#include <QVector>

namespace Sft {
class Sample;

class SampleList {
public:
  //! Deconstructor
  ~SampleList();

  //! Removes all samples
  void clear();

  //! Creates a new sample
  Sample *create(const QString &name);

  //! Removes a sample
  void remove(Sample *);

  //! Returns a sample by its ID
  Sample *findById(const QString &id) const;

  //! Adds a sample
  void add(Sample *sample) { mSamples.push_back(sample); }

  //! Adjusts the instrument quality of all instruments to fit in DSP RAM
  void autoAdjustSampleQuality(uint32_t maxSize = 0x8000);

protected:
  //! Returns the name as unique name
  QString uniqueName(const QString &name) const;

  //! Returns the id as unique id
  QString uniqueId(const QString &name) const;

  //! List of all samples
  QVector<Sample*> mSamples;
};

}