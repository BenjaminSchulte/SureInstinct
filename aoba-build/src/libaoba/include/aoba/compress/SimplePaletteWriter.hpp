#pragma once

#include "AbstractWriter.hpp"

namespace Aoba {
class Project;

class SimplePaletteWriter : public AbstractWriter {
public:
  SimplePaletteWriter(Project *project) : AbstractWriter(project) {}

  bool write(const char *data, uint32_t size) override;
  bool finalize() override;

private:
  void writeByte(uint8_t byte);
  void writeColor(uint16_t color);
  void flushRepeat();

  int mLastColorByte = -1;

  uint8_t mLastR=0;
  uint8_t mLastG=0;
  uint8_t mLastB=0;

  uint8_t mRepeatCounter=0;
};

}