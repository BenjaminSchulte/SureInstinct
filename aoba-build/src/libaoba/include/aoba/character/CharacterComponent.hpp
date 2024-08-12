#pragma once

#include <QString>
#include <yaml-cpp/yaml.h>

namespace Aoba {
class Character;
class CharacterAssetSet;

class CharacterComponent {
public:
  //! Constructor
  CharacterComponent(const QString &id) : mId(id) {}

  //! Deconstructor
  virtual ~CharacterComponent() = default;  

  //! Returns the ID
  inline const QString &id() const { return mId; }

  //! Returns the type ID
  virtual QString typeId() const = 0;

  //! Loads the component
  virtual bool load(Character *level, YAML::Node &config) const = 0;

  //! Builds the component
  virtual bool build(Character*, FMA::linker::LinkerBlock *block) const = 0;

  //! Builds this component (only called once, but is also called if no character exists)
  virtual bool staticBuild(CharacterAssetSet *) const { return true; }

protected:
  //! The ID
  QString mId;
};
  
}