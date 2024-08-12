#pragma once

#include <QBuffer>
#include <aoba/compress/AbstractWriter.hpp>

namespace Aoba {

class MapTilesWriter : public AbstractWriter {
public:
  MapTilesWriter(Project *project);

  bool write(const char *data, uint32_t size) override;

protected:
  struct Reference {
    Reference() : offset(0), size(0) {}
    uint32_t offset;
    uint32_t size;
  };

  uint32_t writeTile(const char *input, uint32_t index, uint32_t x, uint32_t y, uint16_t w, uint32_t h);
  bool writeTileDirect(const char *input, uint32_t index);
  
  Reference findReference(const char *input, int32_t x, int32_t y, uint16_t w, uint16_t h, int ox, int oy) const;
  uint32_t countReferenceSize(const char *input, uint32_t i, uint32_t currentIndex, uint32_t x, uint16_t w, uint16_t h) const;

  QBuffer mStream;
};

}