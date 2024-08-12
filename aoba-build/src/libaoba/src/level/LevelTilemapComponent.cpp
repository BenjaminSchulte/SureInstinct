#include <aoba/log/Log.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <aoba/level/Level.hpp>
#include <aoba/level/LevelConfig.hpp>
#include <aoba/level/LevelTilemapComponent.hpp>
#include <aoba/tilemap/Tilemap.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool LevelTilemapComponent::resize(Level *level, int left, int top, int right, int bottom, const QSize &) const { 
  return tilemap(level)->resize(left, top, right, bottom);
}

// -----------------------------------------------------------------------------
bool LevelTilemapComponent::save(Level *level, YAML::Emitter &root) const {
  root << YAML::Key << id().toStdString() << YAML::Value << this->tilemap(level)->id().toStdString();
  return true;
}

// -----------------------------------------------------------------------------
void LevelTilemapComponent::setTilemap(Level *level, Tilemap *tilemap) const {
  LevelTilemap copy = level->properties()[mId].value<LevelTilemap>();
  copy.tilemap = tilemap;
  
  QVariant value;
  value.setValue(copy);
  level->properties().insert(mId, value);
}

// -----------------------------------------------------------------------------
bool LevelTilemapComponent::load(Level *level, YAML::Node &config) const {
  LevelTilemap tilemap;

  QString tilemapId(config[id().toStdString()].as<std::string>().c_str());
  tilemap.tilemap = mTilemaps->get(tilemapId);
  if (tilemap.tilemap == nullptr) {
    Aoba::log::error("Could not find any tilemap with the ID: " + tilemapId);
    return false;
  }

  QVariant value;
  value.setValue(tilemap);
  level->properties().insert(mId, value);
  return true;
}

// -----------------------------------------------------------------------------
Tilemap *LevelTilemapComponent::tilemap(Level *level) const {
  return level->properties()[mId].value<LevelTilemap>().tilemap;
}

// -----------------------------------------------------------------------------
bool LevelTilemapComponent::build(Level *level, FMA::linker::LinkerBlock *block) const {
  Tilemap *tilemap = this->tilemap(level);
  block->write(FMA::symbol::ReferencePtr(new FMA::symbol::SymbolReference(tilemap->symbolName().toStdString())), 3);
  return true;
}
