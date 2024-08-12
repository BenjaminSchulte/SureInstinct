#pragma once

#include <QSize>
#include <QString>
#include <yaml-cpp/yaml.h>

namespace FMA {
namespace linker {
class LinkerBlock;
}
}

namespace Aoba {
class Level;

class LevelComponent {
public:
  //! Constructor
  LevelComponent(const QString &id) : mId(id) {}

  //! Deconstructor
  virtual ~LevelComponent() = default;  

  //! Returns the ID
  inline const QString &id() const { return mId; }

  //! Returns the type ID
  virtual QString typeId() const = 0;

  //! Loads the component
  virtual bool load(Level *level, YAML::Node &config) const = 0;

  //! Saves the component
  virtual bool save(Level *Level, YAML::Emitter &root) const = 0;

  //! Resizes content
  virtual bool resize(Level *level, int left, int top, int right, int bottom, const QSize &newSize) const {
    (void)level; (void)left; (void)top; (void)right; (void)bottom; (void)newSize;
    return true;
  };

  //! Builds the component
  virtual bool build(Level*, FMA::linker::LinkerBlock*) const {
    return true;
  }

  //! Returns FMA code
  virtual QString getFmaCode(const Level*) const {
    return "";
  }

protected:
  //! The ID
  QString mId;
};
  
}