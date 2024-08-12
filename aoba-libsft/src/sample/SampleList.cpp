#include <QDebug>
#include <sft/brr/BrrSample.hpp>
#include <sft/sample/Sample.hpp>
#include <sft/sample/SampleList.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
SampleList::~SampleList() {
  clear();
}

// -----------------------------------------------------------------------------
void SampleList::clear() {
  for (Sample *inst : mSamples) {
    delete inst;
  }

  mSamples.clear();
}

// -----------------------------------------------------------------------------
QString SampleList::uniqueName(const QString &name) const {
  bool retry = true;
  int nextPostfix = 2;
  QString currentName = name;

  while (retry) {
    retry = false;

    QVectorIterator<Sample*> it(mSamples);
    while (it.hasNext()) {
      if (it.next()->name() == currentName) {
        currentName = name + " " + QString::number(nextPostfix++, 10);
        retry = true;
      }
    }
  }

  return currentName;
}

// -----------------------------------------------------------------------------
QString SampleList::uniqueId(const QString &name) const {
  bool retry = true;
  int nextPostfix = 2;
  QString currentName = name;

  while (retry) {
    retry = false;

    QVectorIterator<Sample*> it(mSamples);
    while (it.hasNext()) {
      if (it.next()->name() == currentName) {
        currentName = name + QString::number(nextPostfix++, 10);
        retry = true;
      }
    }
  }

  return currentName;
}

// -----------------------------------------------------------------------------
Sample *SampleList::create(const QString &name) {
  Sample *sample = new Sample(uniqueId(name), uniqueName(name));
  mSamples.push_back(sample);
  return sample;
}

// -----------------------------------------------------------------------------
Sample *SampleList::findById(const QString &id) const {
  for (Sample *inst : mSamples) {
    if (inst->id() == id) {
      return inst;
    }
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
void SampleList::remove(Sample *sample) {
  if (!mSamples.contains(sample)) {
    return;
  }

  delete sample;
  mSamples.removeAll(sample);
}

// -----------------------------------------------------------------------------
void SampleList::autoAdjustSampleQuality(uint32_t maxSize) {
  uint32_t totalSize = 0;
  uint32_t fixedSize = 0;


  // TODO: don't calculate brr samples for size estimation. Use duration * quality * brrsize or something like that
  for (Sample *inst : mSamples) {
    if (inst->isRawBrrSample()) {
      fixedSize += inst->brrSample()->streamLengthInBytes();
    } else {
      totalSize += inst->brrSample()->streamLengthInBytes();
    }
  }

  if (fixedSize >= maxSize) {
    return;
  }

  maxSize -= fixedSize;

  double factor = (double)maxSize / (double)totalSize;
  if (factor >= 1) {
    return;
  }

  for (Sample *inst : mSamples) {
    if (inst->isRawBrrSample()) {
      continue;
    }

    inst->setCompressionFactor(inst->compressionFactor() * factor);
  }
}