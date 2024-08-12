#pragma once

#include <QString>

namespace Aoba {

class SongBuilderAdapter {
public:
  //! Constructor
  SongBuilderAdapter() = default;

  //! Deconstructor
  virtual ~SongBuilderAdapter() = default;

  //! Resolves the instrument ID (-1 on error)
  virtual int16_t resolveInstrumentId(const QString &id) const = 0;
};

}