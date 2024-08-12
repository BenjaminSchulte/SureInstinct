#pragma once

#include <QString>
#include <yaml-cpp/yaml.h>

namespace FMA {
namespace linker {
class LinkerBlock;
}
}

namespace Aoba {
class PaletteGroup;

class PaletteGroupComponent {
public:
  //! Constructor
  PaletteGroupComponent(const QString &id) : mId(id) {}

  //! Deconstructor
  virtual ~PaletteGroupComponent() = default;  

  //! Returns the ID
  inline const QString &id() const { return mId; }

  //! Loads the component
  virtual bool load(PaletteGroup *level, int index, int row, YAML::Node &config) const = 0;

  //! Builds the component
  virtual bool build(PaletteGroup*, FMA::linker::LinkerBlock*) const {
    return true;
  }

protected:
  //! The ID
  QString mId;
};
  
}