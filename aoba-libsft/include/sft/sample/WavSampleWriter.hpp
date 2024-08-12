#pragma once

#include "SampleWriter.hpp"

namespace Sft {
class Sample;

class WavSampleWriter : public SampleWriter {
public:
  //! Constructor
  WavSampleWriter(Sample *sample) : SampleWriter(sample) {}

  //! Loads the sample
  bool save(const QString &filename) const override;
};

}