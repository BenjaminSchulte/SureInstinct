#pragma once

#include "./LevelComponent.hpp"

namespace Aoba {
class PaletteGroup;
class PaletteGroupSet;

struct LevelPalette {
  QString mId;
  PaletteGroup *mPalette;
};

class LevelPaletteGroupComponent : public LevelComponent {
public:
  //! Constructor
  LevelPaletteGroupComponent(const QString &id, PaletteGroupSet *paletteGroups)
    : LevelComponent(id)
    , mPaletteGroups(paletteGroups)
  {}

  //! The type id
  static QString TypeId() { return "LevelPaletteGroupComponent"; }

  //! Returns the type ID
  QString typeId() const override { return TypeId(); }

  //! Loads the component
  bool load(Level *level, YAML::Node &config) const override;

  //! Saves the component
  bool save(Level *Level, YAML::Emitter &root) const override;
  
  //! Builds the component
  bool build(Level *level, FMA::linker::LinkerBlock *block) const override;

  //! Sets the palette
  void setPalette(Level *level, Aoba::PaletteGroup *palette) const;

private:
  //! PaletteGroups
  PaletteGroupSet *mPaletteGroups;
};
  
}

Q_DECLARE_METATYPE(Aoba::LevelPalette);