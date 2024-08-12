#pragma once

#include <QDir>
#include <QMap>
#include <QSharedPointer>
#include "TileFlagGroupConfig.hpp"

namespace Aoba {

class TileFlagGroupBooleanParameter : public TileFlagGroupParameter {
public:
  //! Constructor
  TileFlagGroupBooleanParameter(const QString &name) : TileFlagGroupParameter(name) {}

  //! Writes the CSV header
  void writeCsvHeader(const QString &id, CsvWriter &csv) const override;

  //! Writes the CSV data
  void writeTile(const QString &id, const QVariant &, CsvWriter &csv) const override;

  //! Loads a tile
  QVariant loadTile(const QString &id, CsvReader &csv, bool &ok) const override;

  //! Loads a tile
  QString hashTile(const QVariant &) const override;

  //! Returns a numeric value
  uint64_t numericValue(const QVariant &) const override;

  //! Returns the true value
  inline uint64_t trueValue() const { return mTrueValue; }

  //! Returns the false value
  inline uint64_t falseValue() const { return mFalseValue; }

  //! Sets the true value
  inline void setTrueValue(uint64_t value) { mTrueValue = value; }

  //! Sets the false value
  inline void setFalseValue(uint64_t value) { mFalseValue = value; }

  //! Sets the default value
  inline void setDefaultValue(bool value) { mDefaultValue = value; }

protected:
  //! All options
  uint64_t mFalseValue = 0;

  //! All options
  uint64_t mTrueValue = 0;

  //! The default value
  bool mDefaultValue;
};

}
