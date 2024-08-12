#pragma once

#include <QByteArray>
#include <QVariant>

namespace Aoba {
class Project;

class AbstractWriter {
public:
  AbstractWriter(Project *project) : mProject(project) {}
  virtual ~AbstractWriter() = default;

  virtual bool write(const char *data, uint32_t size) = 0;
  bool writeArray(const QByteArray &data) {
    const char *buffer = data.constData();
    return write(buffer, data.size());
  }
  virtual bool finalize() { return true; }
  virtual const QByteArray &buffer() const { return mBuffer; }

protected:
  Project *mProject;
  QByteArray mBuffer;
};

}