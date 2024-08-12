#include <aoba/log/Log.hpp>
#include <aoba/tileset/TileFlagGroupEnumParameter.hpp>
#include <aoba/csv/CsvReader.hpp>
#include <aoba/csv/CsvWriter.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
void TileFlagGroupEnumParameter::writeCsvHeader(const QString &id, CsvWriter &csv) const {
  csv << id;
}

// -----------------------------------------------------------------------------
void TileFlagGroupEnumParameter::writeTile(const QString &, const QVariant &value, CsvWriter &csv) const {
  csv << value.toString();
}

// -----------------------------------------------------------------------------
QVariant TileFlagGroupEnumParameter::loadTile(const QString &id, CsvReader &csv, bool &) const {
  return csv.get(id);
}

// -----------------------------------------------------------------------------
QString TileFlagGroupEnumParameter::hashTile(const QVariant &value) const {
  return value.toString();
}

// -----------------------------------------------------------------------------
uint64_t TileFlagGroupEnumParameter::numericValue(const QVariant &value) const {
  QString key = value.toString();

  if (key.isEmpty()) {
    key = mDefaultValue;
  }
  if (key.isEmpty()) {
    Aoba::log::warn("TileFlagGroup parameter has no default value and no value");
    return 0;
  }

  if (!mOptions.contains(key)) {
    Aoba::log::warn("Tile has unknown enum value: " + key);
    return 0;
  }

  return mOptions[key].value();
}