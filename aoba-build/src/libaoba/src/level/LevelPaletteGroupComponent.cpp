#include <aoba/log/Log.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <aoba/level/Level.hpp>
#include <aoba/level/LevelConfig.hpp>
#include <aoba/level/LevelPaletteGroupComponent.hpp>
#include <aoba/palette/PaletteGroup.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
void LevelPaletteGroupComponent::setPalette(Level *level, Aoba::PaletteGroup *palette) const {
  LevelPalette copy = level->properties()[mId].value<LevelPalette>();
  copy.mId = palette->id();
  copy.mPalette = palette;
  
  QVariant value;
  value.setValue(copy);
  level->properties().insert(mId, value);
}

// -----------------------------------------------------------------------------
bool LevelPaletteGroupComponent::save(Level *level, YAML::Emitter &root) const {
  const LevelPalette &palette = level->properties()[mId].value<LevelPalette>();
  root << YAML::Key << id().toStdString() << YAML::Value << palette.mId.toStdString();
  return true;
}

// -----------------------------------------------------------------------------
bool LevelPaletteGroupComponent::load(Level *level, YAML::Node &config) const {
  LevelPalette palette;

  palette.mId = Yaml::asString(config[id().toStdString()]);
  palette.mPalette = mPaletteGroups->get(palette.mId);
  if (palette.mPalette == nullptr) {
    Aoba::log::error("Could not find any paletteGroup with the ID: " + palette.mId + " for level " + level->id());
    return false;
  }

  QVariant value;
  value.setValue(palette);
  level->properties().insert(mId, value);
  return true;
}

// -----------------------------------------------------------------------------
bool LevelPaletteGroupComponent::build(Level *level, FMA::linker::LinkerBlock *block) const {
  const LevelPalette &palette = level->properties()[mId].value<LevelPalette>();
  block->write(FMA::symbol::ReferencePtr(new FMA::symbol::SymbolReference(palette.mPalette->assetSymbolName().toStdString())), 2);
  return true;
}
