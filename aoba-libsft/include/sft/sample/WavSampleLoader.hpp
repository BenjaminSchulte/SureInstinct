#pragma once

#include "SampleLoader.hpp"

namespace Sft {
class Sample;

class WavSampleLoader : public SampleLoader {
public:
  //! Constructor
  WavSampleLoader(Sample *sample) : SampleLoader(sample) {}

  //! Loads the sample
  bool load(const QString &filename) const override;
};

}