#include <QDebug>
#include <sft/sample/Sample.hpp>
#include <sft/instrument/Instrument.hpp>
#include <sft/instrument/InstrumentList.hpp>
#include <sft/types/ColorPalette.hpp>

using namespace Sft;

// -----------------------------------------------------------------------------
InstrumentList::~InstrumentList() {
  clear();
}

// -----------------------------------------------------------------------------
QString InstrumentList::uniqueName(const QString &name) const {
  bool retry = true;
  int nextPostfix = 2;
  QString currentName = name;

  while (retry) {
    retry = false;

    QVectorIterator<InstrumentPtr> it(mInstruments);
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
void InstrumentList::clear() {
  mInstruments.clear();
}

// -----------------------------------------------------------------------------
InstrumentPtr InstrumentList::create(Sample *sample) {
  InstrumentPtr instrument(new Instrument(sample, sample->id(), sample->name()));
  instrument->setColor(ColorPalette::color(mInstruments.size() % ColorPalette::numColors()));
  mInstruments.push_back(instrument);

  return instrument;
}

// -----------------------------------------------------------------------------
InstrumentPtr InstrumentList::findById(const QString &name) const {
  for (const InstrumentPtr &inst : mInstruments) {
    if (inst->id() == name) {
      return inst;
    }
  }

  return nullptr;
}
