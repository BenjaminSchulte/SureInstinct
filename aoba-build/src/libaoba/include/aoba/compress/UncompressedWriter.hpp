#pragma once

#include "AbstractWriter.hpp"

namespace Aoba {
class Project;

class UncompressedWriter : public AbstractWriter {
public:
  UncompressedWriter(Project *project) : AbstractWriter(project) {}

  bool write(const char *data, uint32_t size) {
    mBuffer.append(data, size);
    return true;
  }
};

}