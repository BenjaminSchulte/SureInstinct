#include <QJsonArray>
#include <QJsonDocument>
#include <QDir>
#include <aoba/log/Log.hpp>
#include <aoba/json/JsonObjectReader.hpp>
#include <aoba/log/Log.hpp>

using namespace Aoba;

// ----------------------------------------------------------------------------
QString JsonValueReader::typeName() const {
  switch (mValue.type()) {
    case QJsonValue::Type::Array: return "array";
    case QJsonValue::Type::Bool: return "bool";
    case QJsonValue::Type::Double: return "double";
    case QJsonValue::Type::Null: return "null";
    case QJsonValue::Type::Object: return "object";
    case QJsonValue::Type::String: return "string";
    case QJsonValue::Type::Undefined: return "undefined";
    default: return "unknown";
  }
}

// ----------------------------------------------------------------------------
QVector<JsonValueReader> JsonValueReader::array() const {
  if (!isArray()) return {};

  QVector<JsonValueReader> result;
  for (const auto &valueRef : mValue.toArray()) {
    result.push_back(JsonValueReader(valueRef, mLog));
  }

  return result;
}

// ----------------------------------------------------------------------------
JsonObjectReader JsonValueReader::object() const {
  return JsonObjectReader(mValue.toObject(), mLog);
}

// ----------------------------------------------------------------------------
JsonValueReader JsonObjectReader::value() {
  return JsonValueReader(mObject, mLog);
}

// ----------------------------------------------------------------------------
QVector<JsonValueReader> JsonObjectReader::array(const char *id) const {
  if (!isArray(id)) return {};

  QVector<JsonValueReader> result;
  for (const auto &valueRef : mObject[id].toArray()) {
    result.push_back(JsonValueReader(valueRef, mLog));
  }

  return result;
}

// ----------------------------------------------------------------------------
void JsonObjectReader::defaultWarning(const char *childId, const char *expectedType) const {
  QString stringId = childId;
  return defaultWarning(stringId, expectedType);
}

// ----------------------------------------------------------------------------
void JsonObjectReader::defaultWarning(const QString &childId, const char *expectedType) const {
  if (!has(childId)) {
    return;
  }

  QString isType = JsonValueReader(mObject[childId]).typeName();

  if (mLog) {
    mLog->log(Aoba::LOG_WARN, "JSON member " + childId + " expects to be " + expectedType + ", but is " + isType);
  } else {
    Aoba::log::debug("JSON member " + childId + " expects to be " + expectedType + ", but is " + isType);
  }
}

// ----------------------------------------------------------------------------
void JsonObjectReader::preprocess() {
}