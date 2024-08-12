#pragma once

#include <QDir>
#include <QMap>
#include <QSharedPointer>
#include "TileFlagGroupConfig.hpp"

namespace Aoba {

struct TileFlagGroupEnumParameterValue {
  //! Constructor
  TileFlagGroupEnumParameterValue() {}

  //! Constructor
  TileFlagGroupEnumParameterValue(uint64_t value, const QString &name)
    : mValue(value)
    , mName(name)
  {}

  //! Returns the value
  inline uint64_t value() const { return mValue; }

  //! Returns the name
  inline const QString &name() const { return mName; }

private:
  //! The value
  uint64_t mValue;

  //! The name
  QString mName;
};

class TileFlagGroupEnumParameter : public TileFlagGroupParameter {
public:
  //! Constructor
  TileFlagGroupEnumParameter(const QString &name) : TileFlagGroupParameter(name) {}

  //! Adds an option
  inline TileFlagGroupEnumParameterValue &addOption(const QString &id, uint64_t value, const QString &name) {
    mOptions.insert(id, TileFlagGroupEnumParameterValue(value, name));
    return mOptions[id];
  }

  //! Returns all options
  inline const QMap<QString, TileFlagGroupEnumParameterValue> &options() const { return mOptions; }

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

  //! Sets the default value
  inline void setDefaultValue(const QString &value) { mDefaultValue = value; }

protected:
  //! All options
  QMap<QString, TileFlagGroupEnumParameterValue> mOptions;

  //! The default value
  QString mDefaultValue;
};

}
