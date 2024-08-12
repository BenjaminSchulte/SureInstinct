#include <aoba/level/Level.hpp>
#include <aoba/level/LevelConfig.hpp>
#include <aoba/level/LevelComponent.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
LevelAssetSet::~LevelAssetSet() {
  for (LevelComponent *component : mComponents) {
    delete component;
  }
}

// -----------------------------------------------------------------------------
void LevelAssetSet::addComponent(LevelComponent *comp) {
  mComponents.push_back(comp);
}

// -----------------------------------------------------------------------------
LevelComponent *LevelAssetSet::getComponent(const QString &type) const {
  for (LevelComponent *comp : mComponents) {
    if (comp->typeId() == type) {
      return comp;
    }
  }

  return nullptr;
}

