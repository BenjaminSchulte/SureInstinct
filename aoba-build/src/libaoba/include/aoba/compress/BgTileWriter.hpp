#pragma once

#include "AbstractWriter.hpp"

class QDataStream;
class QBuffer;

namespace Aoba {
class BgTileCompressor;

class BgTileWriter : public AbstractWriter {
public:
  BgTileWriter(Project*);
  ~BgTileWriter() override;

  static const char *Id() { return "bgtiles"; }

  bool write(const char *data, uint32_t size) override;
  void writePair(char low, char high);
  bool finalize() override;

  static QByteArray compressed(Project *project, const QByteArray &data) {
    BgTileWriter writer(project);
    if (!writer.writeArray(data)) { return QByteArray(); }
    if (!writer.finalize()) { return QByteArray(); }
    return writer.buffer();
  }

private:
  QByteArray *mLow;
  QBuffer *mLowBuffer;

  QByteArray *mHigh;
  QBuffer *mHighBuffer;

  uint32_t mSize;
};

}