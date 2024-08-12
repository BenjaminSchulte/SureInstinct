#pragma once

#include <QJsonObject>
#include <QStringList>

namespace Aoba {
class Log;
struct JsonObjectReader;

struct JsonValueReader {
  JsonValueReader() : mValue(), mLog(nullptr) {}
  JsonValueReader(QJsonValue value, Log *project=nullptr) : mValue(value), mLog(project) {}

  inline bool isString() const { return mValue.isString(); }
  inline bool isObject() const { return mValue.isObject(); }
  inline bool isArray() const { return mValue.isArray(); }
  inline bool isInteger() const { return mValue.isDouble(); }

  inline QString string() const { return mValue.toString(); }
  QVector<JsonValueReader> array() const;
  inline int integer() const { return mValue.toInt(); }
  JsonObjectReader object() const;
  inline Log *project() const { return mLog; }

  QString typeName() const;

protected:
  QJsonValue mValue;
  Log *mLog;
};

struct JsonObjectReader {
  JsonObjectReader(QJsonObject object, Log *project=nullptr) : mObject(object), mLog(project) {
    preprocess();
  }

  JsonValueReader value();

  inline QStringList keys() const {
    return mObject.keys();
  }


  inline bool isArray(const char *id) const {
    return mObject.contains(id) && mObject[id].isArray();
  }
  QVector<JsonValueReader> array(const char *id) const;


  inline bool has(const QString &id) const { return mObject.contains(id); }
  inline bool has(const char *id) const { return mObject.contains(id); }

  inline JsonValueReader get(const char *id) const {
    return JsonValueReader(mObject[id], mLog);
  }

  inline JsonValueReader get(const QString &id) const {
    return JsonValueReader(mObject[id], mLog);
  }

  // number
  inline bool isNumber(const char *id) const {
    return mObject.contains(id) && mObject[id].isDouble();
  }
  int number(const char *id, int defaultValue=0) const {
    if (!isNumber(id)) { defaultWarning(id, "number"); return defaultValue; }
    return mObject[id].toInt();
  }

  // boolean
  inline bool isBoolean(const char *id) const {
    return mObject.contains(id) && mObject[id].isBool();
  }
  bool boolean(const char *id, bool defaultValue=false) const {
    if (!isBoolean(id)) { defaultWarning(id, "boolean"); return defaultValue; }
    return mObject[id].toBool();
  }

  // string
  inline bool isString(const char *id) const {
    return mObject.contains(id) && mObject[id].isString();
  }
  inline bool isString(const QString &id) const {
    return mObject.contains(id) && mObject[id].isString();
  }
  QString string(const char *id, const QString &defaultValue="") const {
    if (!isString(id)) { defaultWarning(id, "string"); return defaultValue; }
    return mObject[id].toString();
  }
  QString string(const QString &id, const QString &defaultValue="") const {
    if (!isString(id)) { defaultWarning(id, "string"); return defaultValue; }
    return mObject[id].toString();
  }

  // object
  inline bool isObject(const char *id) const {
    return mObject.contains(id) && mObject[id].isObject();
  }
  inline bool isObject(const QString &id) const {
    return mObject.contains(id) && mObject[id].isObject();
  }
  inline JsonObjectReader object(const QString &id) const {
    QByteArray ba(id.toLocal8Bit());
    return object(ba.data());
  }
  JsonObjectReader object(const char *id) const {
    if (!isObject(id)) { return JsonObjectReader(QJsonObject()); }
    return JsonObjectReader(mObject[id].toObject(), mLog);
  }

  inline QJsonObject &object() { return mObject; }
  inline const QJsonObject &object() const { return mObject; }
  inline Log *project() const { return mLog; }

  void includeFile(const QString &);

protected:
  void preprocess();
  void defaultWarning(const QString &childId, const char *expectedType) const;
  void defaultWarning(const char *childId, const char *expectedType) const;

  QJsonObject mObject;
  Log *mLog;
};

}