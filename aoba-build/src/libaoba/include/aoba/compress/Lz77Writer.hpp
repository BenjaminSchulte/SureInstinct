#pragma once

#include "AbstractWriter.hpp"

namespace Aoba {

class Lz77Writer : public AbstractWriter {
public:
  Lz77Writer(Project *project, uint32_t maxBytesPerSection = 0x2000)
    : AbstractWriter(project)
    , mMaxBytesPerSection(maxBytesPerSection)
    , mCurrentBufferOffset(0)
  {}

  static const char *Id() { return "lzw"; }

  bool write(const char *data, uint32_t size) override;

  static QByteArray compressed(Project *project, const QByteArray &data) {
    Lz77Writer writer(project);
    if (!writer.writeArray(data)) { return QByteArray(); }
    return writer.buffer();
  }

protected:
  void writePart(const char *data, uint32_t size, bool hasMore);
 
  uint32_t mMaxBytesPerSection;

  uint32_t mCurrentBufferOffset;
};

}