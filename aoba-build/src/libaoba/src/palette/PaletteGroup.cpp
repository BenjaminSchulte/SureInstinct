#include <aoba/palette/PaletteGroup.hpp>
#include <aoba/palette/Palette.hpp>
#include <aoba/palette/PaletteGroupComponent.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <yaml-cpp/yaml.h>

using namespace Aoba;

namespace {
  int nextPaletteGroupId = 0;
}

// -----------------------------------------------------------------------------
PaletteGroup::PaletteGroup(PaletteGroupSet *set, const QString &id)
  : Asset(set, id)
  , mAssetSymbolId(nextPaletteGroupId++)
{
  set->add(this);
}

// -----------------------------------------------------------------------------
PaletteGroup::~PaletteGroup() {
  for (Palette *p : mGeneratedPalettes) {
    delete p;
  }
}

// -----------------------------------------------------------------------------
bool PaletteGroup::reload() {
  return true;
}

// -----------------------------------------------------------------------------
QString PaletteGroup::assetSymbolName() const {
  return "__asset_palette_group" + QString::number(mAssetSymbolId, 10);
}

// -----------------------------------------------------------------------------
bool PaletteGroup::build() {
  auto *block = mAssetSet->paletteSet()->createLinkerBlock(assetSymbolName());
  uint16_t numPalettes = mMaximumRecord + 1;
  block->write(&numPalettes, 2);

  uint16_t empty = 0;
  
  for (int i=0; i<=mMaximumRecord; i++) {
    if (mPalettes.contains(i)) {
      block->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(mPalettes[i]->assetSymbolName().toStdString())), 2);
    } else {
      block->write(&empty, 2);
    }
  }

  for (PaletteGroupComponent *comp : mAssetSet->components()) {
    if (!comp->build(this, block)) {
      Aoba::log::error("Unable to build palette component");
      return false;
    }
  }


  return true;
}

// -----------------------------------------------------------------------------
QString PaletteGroup::getFmaCode() const {
  return "";
}

// -----------------------------------------------------------------------------
Palette *PaletteGroup::add(int index, Palette *p) {
  mPalettes.insert(index, p);
  if (index > mMaximumRecord) {
    mMaximumRecord = index;
  }

  return p;
}

// -----------------------------------------------------------------------------
Palette *PaletteGroup::add(int index, const QString &file, int row) {
  YAML::Node config;
  try {
    QString infoFile(file);
    infoFile.replace(".png", ".yml");
    config = YAML::LoadFile(infoFile.toStdString().c_str());
  } catch(YAML::BadFile&) {
  }

  Palette *p = new Palette(mAssetSet->paletteSet(), file + "#" + QString::number(row), file, row == -1 ? index : row);
  mGeneratedPalettes.push_back(p);

  for (PaletteGroupComponent *comp : mAssetSet->components()) {
    if (!comp->load(this, index, row, config)) {
      Aoba::log::warn("Unable to load palette component configuration");
    }
  }

  return add(index, p);
}
