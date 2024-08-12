#pragma once

#include <QVector>
#include "JsonObjectWriter.hpp"

namespace Aoba {

struct JsonStringWriterStreamConfig {
  JsonStringWriterStreamConfig()
    : numSpacesAfterComma(1)
    , numSpacesNewLine(2)
    , numSpacesAfterColon(1)
    , spaceIndenting(2)
    , newLineEveryNthItem(1)
  {}

  JsonStringWriterStreamConfig indent() const {
    JsonStringWriterStreamConfig copy(*this);
    copy.numSpacesNewLine += copy.spaceIndenting;
    return copy;
  }
  
  int numSpacesAfterComma;
  int numSpacesNewLine;
  int numSpacesAfterColon;
  int spaceIndenting;
  int newLineEveryNthItem;
};

class JsonStringWriterStream {
public:
  JsonStringWriterStream(const QString &prefix, const QString &postfix, JsonStringWriterStreamConfig config);
  ~JsonStringWriterStream();

  void commaNewLine();
  void newLine();
  void write(const QString &value);
  void write(JsonWriter *writer, JsonStringWriterStream *stream);

  QString get() const;
  inline const JsonStringWriterStreamConfig &config() const { return mConfig; }
  inline JsonStringWriterStreamConfig &config() { return mConfig; }

protected:
  void addRequiredCommaNewLine();

  QString mPrefix;
  QString mPostfix;

  JsonStringWriterStreamConfig mConfig;
  QVector<JsonWriter*> mMembers;
  QVector<QString> mConstantStrings;
  QVector<JsonStringWriterStream*> mVariableStrings;
  int mNewLineCounter;
  bool mRequireCommaNewLine;
};

class JsonStringArrayWriter : public JsonArrayWriter {
public:
  JsonStringArrayWriter(JsonStringWriterStreamConfig config=JsonStringWriterStreamConfig());

  void disableLineBreaks() override;
  void addLineBreaksEveryNthItem(int) override;

  void string(const QString &value) override;
  void boolean(bool value) override;
  void number(int value) override;
  JsonObjectWriter *object() override;
  JsonArrayWriter *array() override;

  inline JsonStringWriterStream &stream() { return mStream; }

protected:
  JsonStringWriterStream mStream;
};

class JsonStringWriter : public JsonObjectWriter {
public:
  JsonStringWriter(JsonStringWriterStreamConfig config=JsonStringWriterStreamConfig());

  void string(const QString &key, const QString &value) override;
  void boolean(const QString &key, bool value) override;
  void number(const QString &key, int value) override;
  JsonObjectWriter *object(const QString &key) override;
  JsonArrayWriter *array(const QString &key) override;

  static QString escapeString(const QString &value);

  inline JsonStringWriterStream &stream() { return mStream; }
  QString get() const;

protected:
  void writeKey(const QString &key);
  JsonStringWriterStream mStream;
};

}