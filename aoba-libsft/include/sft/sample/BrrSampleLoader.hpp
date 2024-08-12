#pragma once

#include "SampleLoader.hpp"

namespace Sft {
class Sample;

class BrrSampleLoader : public SampleLoader {
public:
  //! Constructor
  BrrSampleLoader(Sample *sample) : SampleLoader(sample) {}

  //! Loads the sample
  bool load(const QString &filename) const override;
};

}