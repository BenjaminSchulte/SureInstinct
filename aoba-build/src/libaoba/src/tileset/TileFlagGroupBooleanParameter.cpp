#include <aoba/log/Log.hpp>
#include <aoba/tileset/TileFlagGroupBooleanParameter.hpp>
#include <aoba/csv/CsvReader.hpp>
#include <aoba/csv/CsvWriter.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
void TileFlagGroupBooleanParameter::writeCsvHeader(const QString &id, CsvWriter &csv) const {
  csv << id;
}

// -----------------------------------------------------------------------------
void TileFlagGroupBooleanParameter::writeTile(const QString &, const QVariant &value, CsvWriter &csv) const {
  csv << (value.toBool() ? 1 : 0);
}

// -----------------------------------------------------------------------------
QVariant TileFlagGroupBooleanParameter::loadTile(const QString &id, CsvReader &csv, bool &) const {
  return csv.get(id).toInt();
}

// -----------------------------------------------------------------------------
QString TileFlagGroupBooleanParameter::hashTile(const QVariant &value) const {
  return value.toBool() ? "1" : "0";
}

// -----------------------------------------------------------------------------
uint64_t TileFlagGroupBooleanParameter::numericValue(const QVariant &value) const {
  bool key = value.toBool();
  if (key) {
    return mTrueValue;
  } else {
    return mFalseValue;
  }
}