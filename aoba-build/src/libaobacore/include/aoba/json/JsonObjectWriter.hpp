#pragma once

#include <QString>

namespace Aoba {
class JsonObjectWriter; 

class JsonWriter {
public:
  JsonWriter() = default;
  virtual ~JsonWriter() = default;
};

class JsonArrayWriter : public JsonWriter {
public:
  virtual void disableLineBreaks() {}
  virtual void addLineBreaksEveryNthItem(int) {}

  virtual void newLine() {}
  virtual void string(const QString &value) = 0;
  virtual void number(int value) = 0;
  virtual void boolean(bool value) = 0;
  virtual JsonObjectWriter *object() = 0;
  virtual JsonArrayWriter *array() = 0;
};

class JsonObjectWriter : public JsonWriter {
public:
  virtual void enableLineBreaks() {}
  virtual void disableLineBreaks() {}

  virtual void newLine() {}
  virtual void string(const QString &key, const QString &value) = 0;
  virtual void number(const QString &key, int value) = 0;
  virtual void boolean(const QString &key, bool value) = 0;
  virtual JsonObjectWriter *object(const QString &key) = 0;
  virtual JsonArrayWriter *array(const QString &key) = 0;
};

}