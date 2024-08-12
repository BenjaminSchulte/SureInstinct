#pragma once

#include <QDataStream>

namespace Sft {

class DataStream : public QDataStream {
public:
  //! Constructor
  DataStream() : QDataStream() {}

  //! Constructor
  DataStream(QIODevice *d) : QDataStream(d) {}

  //! Seek to position
  void seek(unsigned int pos) {
    device()->seek(pos);
    setDevice(device());
  }

  //! Reads a string
  QString readString(int length) {
    char *data = reinterpret_cast<char*>(malloc(length + 1));
    data[length] = 0;
    readRawData(data, length);
    QString result(data);
    free(data);
    return result;
  }

  //! Reads a trimmed string
  inline QString readTrimmedString(int length) {
    return readString(length).trimmed();
  }

  //! Reads a value
  template<typename T> T read() {
    T value;
    *this >> value;
    return value;
  }
};

}