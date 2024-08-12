#pragma once

#include <QBuffer>
#include <aoba/compress/AbstractWriter.hpp>

namespace Aoba {
struct BgTile;

class DialogTilesWriter : public AbstractWriter {
public:
  DialogTilesWriter(Project *project);

  bool write(const char *data, uint32_t size) override;

protected:
  QBuffer mStream;

  bool writeCompressedTile(uint32_t &i, const char *data, uint32_t size, uint16_t tile);
};

}