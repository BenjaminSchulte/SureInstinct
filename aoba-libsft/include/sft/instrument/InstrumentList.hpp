#pragma once

#include <QVector>
#include "Instrument.hpp"

namespace Sft {
class Sample;

class InstrumentList {
public:
  //! Deconstructor
  ~InstrumentList();

  //! Removes all instruments
  void clear();

  //! Creates an instrument
  InstrumentPtr create(Sample *sample);

  //! Returns the amount of samples
  inline uint32_t count() const { return mInstruments.count(); }

  //! Returns all instruments
  inline const QVector<InstrumentPtr> &all() const { return mInstruments; }

  //! Returns an instrument by its name
  InstrumentPtr findById(const QString &name) const;
  
  //! Returns the first instrument
  inline InstrumentPtr first() const { return mInstruments.isEmpty() ? nullptr : mInstruments[0]; }

protected:
  //! Returns the name as unique name
  QString uniqueName(const QString &name) const;

  //! List of all instruments
  QVector<InstrumentPtr> mInstruments;
};

}