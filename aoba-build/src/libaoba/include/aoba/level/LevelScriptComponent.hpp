#pragma once

#include "./LevelComponent.hpp"

namespace Aoba {

class LevelScriptComponent : public LevelComponent {
public:
  //! Constructor
  LevelScriptComponent(const QString &id, const QStringList &scripts)
    : LevelComponent(id)
    , mScripts(scripts)
  {}

  //! The type id
  static QString TypeId() { return "LevelScriptComponent"; }

  //! Returns the type ID
  QString typeId() const override { return TypeId(); }

  //! Loads the component
  bool load(Level *level, YAML::Node &config) const override;

  //! Saves the component
  bool save(Level *Level, YAML::Emitter &root) const override;
  
  //! Builds the component
  bool build(Level *level, FMA::linker::LinkerBlock *block) const override;

protected:
  //! List of all scripts to include
  QStringList mScripts;
};
  
}