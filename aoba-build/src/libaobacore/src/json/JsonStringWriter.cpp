#include <aoba/json/JsonStringWriter.hpp>

using namespace Aoba;

// ----------------------------------------------------------------------------
JsonStringWriterStream::JsonStringWriterStream(const QString &prefix, const QString &postfix, JsonStringWriterStreamConfig config)
  : mPrefix(prefix)
  , mPostfix(postfix)
  , mConfig(config)
  , mNewLineCounter(0)
  , mRequireCommaNewLine(false)
{
}

// ----------------------------------------------------------------------------
JsonStringWriterStream::~JsonStringWriterStream() {
  for (auto *member : mMembers) {
    delete member;
  }
}

// ----------------------------------------------------------------------------
void JsonStringWriterStream::commaNewLine() {
  mRequireCommaNewLine = true;
}

// ----------------------------------------------------------------------------
void JsonStringWriterStream::addRequiredCommaNewLine() {
  if (!mRequireCommaNewLine) {
    return;
  }

  mRequireCommaNewLine = false;
  if (mConfig.newLineEveryNthItem > 0 && ++mNewLineCounter == mConfig.newLineEveryNthItem) {
    mNewLineCounter = 0;
    write(",\n" + QString(" ").repeated(mConfig.numSpacesNewLine));
  } else {
    write("," + QString(" ").repeated(mConfig.numSpacesAfterComma));
  }
}

// ----------------------------------------------------------------------------
void JsonStringWriterStream::newLine() {
  write("\n" + QString(" ").repeated(mConfig.numSpacesNewLine));
}

// ----------------------------------------------------------------------------
void JsonStringWriterStream::write(const QString &value) {
  if (value.isEmpty()) {
    return;
  }

  addRequiredCommaNewLine();
  mConstantStrings.push_back(value);
  mVariableStrings.push_back(nullptr);
}

// ----------------------------------------------------------------------------
void JsonStringWriterStream::write(JsonWriter *writer, JsonStringWriterStream *stream) {
  addRequiredCommaNewLine();
  mConstantStrings.push_back("");
  mVariableStrings.push_back(stream);
  mMembers.push_back(writer);
}

// ----------------------------------------------------------------------------
QString JsonStringWriterStream::get() const {
  QVectorIterator<QString> itConst(mConstantStrings);
  QVectorIterator<JsonStringWriterStream*> itStream(mVariableStrings);

  QString result(mPrefix);
  if (!mConstantStrings.isEmpty() && mConfig.newLineEveryNthItem > 0) {
    result += "\n" + QString(" ").repeated(mConfig.numSpacesNewLine);
  }

  while (itConst.hasNext()) {
    const auto &text = itConst.next();
    const auto *stream = itStream.next();

    if (stream != nullptr) {
      result += stream->get();
    } else {
      result += text;
    }
  }

  if (mRequireCommaNewLine && mConfig.newLineEveryNthItem > 0) {
    result += "\n" + QString(" ").repeated(mConfig.numSpacesNewLine - mConfig.spaceIndenting);
  }

  return result + mPostfix;
}


// ----------------------------------------------------------------------------
JsonStringArrayWriter::JsonStringArrayWriter(JsonStringWriterStreamConfig config)
  : mStream("[", "]", config)
{
}

// ----------------------------------------------------------------------------
void JsonStringArrayWriter::disableLineBreaks() {
  mStream.config().newLineEveryNthItem = 0;
}

// ----------------------------------------------------------------------------
void JsonStringArrayWriter::addLineBreaksEveryNthItem(int cells) {
  mStream.config().newLineEveryNthItem = cells;
}

// ----------------------------------------------------------------------------
void JsonStringArrayWriter::string(const QString &value) {
  mStream.write(JsonStringWriter::escapeString(value));
  mStream.commaNewLine();
}

// ----------------------------------------------------------------------------
void JsonStringArrayWriter::boolean(bool value) {
  mStream.write(value ? "true" : "false");
  mStream.commaNewLine();
}

// ----------------------------------------------------------------------------
void JsonStringArrayWriter::number(int value) {
  mStream.write(QString::number(value, 10));
  mStream.commaNewLine();
}

// ----------------------------------------------------------------------------
JsonObjectWriter *JsonStringArrayWriter::object() {
  JsonStringWriter *writer = new JsonStringWriter(mStream.config().indent());
  mStream.write(writer, &writer->stream());
  mStream.commaNewLine();
  return writer;
}

// ----------------------------------------------------------------------------
JsonArrayWriter *JsonStringArrayWriter::array() {
  JsonStringArrayWriter *writer = new JsonStringArrayWriter(mStream.config().indent());
  mStream.write(writer, &writer->stream());
  mStream.commaNewLine();
  return writer;
}


// ----------------------------------------------------------------------------
JsonStringWriter::JsonStringWriter(JsonStringWriterStreamConfig config)
  : mStream("{", "}", config)
{
}

// ----------------------------------------------------------------------------
void JsonStringWriter::string(const QString &key, const QString &value) {
  writeKey(key);
  mStream.write(JsonStringWriter::escapeString(value));
  mStream.commaNewLine();
}

// ----------------------------------------------------------------------------
void JsonStringWriter::boolean(const QString &key, bool value) {
  writeKey(key);
  mStream.write(value ? "true" : "false");
  mStream.commaNewLine();
}

// ----------------------------------------------------------------------------
void JsonStringWriter::number(const QString &key, int value) {
  writeKey(key);
  mStream.write(QString::number(value, 10));
  mStream.commaNewLine();
}

// ----------------------------------------------------------------------------
JsonObjectWriter *JsonStringWriter::object(const QString &key) {
  writeKey(key);
  JsonStringWriter *writer = new JsonStringWriter(mStream.config().indent());
  mStream.write(writer, &writer->stream());
  mStream.commaNewLine();
  return writer;
}

// ----------------------------------------------------------------------------
JsonArrayWriter *JsonStringWriter::array(const QString &key) {
  writeKey(key);
  JsonStringArrayWriter *writer = new JsonStringArrayWriter(mStream.config().indent());
  mStream.write(writer, &writer->stream());
  mStream.commaNewLine();
  return writer;
}

// ----------------------------------------------------------------------------
QString JsonStringWriter::escapeString(const QString &value) {
  return "\"" + value + "\"";
}

// ----------------------------------------------------------------------------
void JsonStringWriter::writeKey(const QString &key) {
  mStream.write(escapeString(key) + ":" + QString(" ").repeated(mStream.config().numSpacesAfterColon));
}

// ----------------------------------------------------------------------------
QString JsonStringWriter::get() const {
  return mStream.get();
}

