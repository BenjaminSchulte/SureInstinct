#include <QDebug>
#include <sft/instrument/Instrument.hpp>
#include <sft/sample/Sample.hpp>
#include <sft/Configuration.hpp>

using namespace Sft;

namespace {
  double AttackDurations[] = {
    4.1,
    2.5,
    1.5,
    1.0,
    0.640,
    0.380,
    0.260,
    0.160,
    0.096,
    0.064,
    0.040,
    0.024,
    0.016,
    0.010,
    0.006,
    0
  };

  double DecayDurations[] = {
    1.2,
    0.740,
    0.440,
    0.290,
    0.180,
    0.110,
    0.074,
    0.037
  };

  double ReleaseDurations[] = {
    std::numeric_limits<double>::infinity(),
    38,
    28,
    24,
    19,
    14,
    12,
    9.4,
    7.1,
    5.9,
    4.7,
    3.5,
    2.9,
    2.4,
    1.8,
    1.5,
    1.2,
    0.880,
    0.740,
    0.590,
    0.440,
    0.370,
    0.290,
    0.220,
    0.180,
    0.150,
    0.110,
    0.092,
    0.074,
    0.055,
    0.037,
    0.018
  };
}


// -----------------------------------------------------------------------------
double InstrumentAdsr::attackDuration() const {
  return AttackDurations[a];
}

// -----------------------------------------------------------------------------
double InstrumentAdsr::decayDuration() const {
  return DecayDurations[d];
}

// -----------------------------------------------------------------------------
double InstrumentAdsr::sustainPercent() const {
  return s / 7.0;
}

// -----------------------------------------------------------------------------
double InstrumentAdsr::releaseDuration() const {
  return ReleaseDurations[r];
}

// -----------------------------------------------------------------------------
Instrument::Instrument(Sample *sample, const QString &id, const QString &name)
  : mSample(sample)
  , mId(id)
  , mName(name)
  , mColor(128, 128, 255)
{
}

// -----------------------------------------------------------------------------
Instrument::~Instrument() {
}

// -----------------------------------------------------------------------------
double Instrument::playbackNoteAdjust() const {
  return mSample->playbackNoteAdjust() + mNoteAdjust;
}