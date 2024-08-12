#include <QRect>
#include <fma/linker/LinkerBlock.hpp>
#include <aoba/character/Character.hpp>
#include <aoba/character/CharacterComponent.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
CharacterAssetSet::CharacterAssetSet(Project *project, const QString &name, uint8_t headerRomBank)
  : LinkedAssetSet<Character>("character", project)
  , mName(name)
  , mHeaderRomBank(headerRomBank)
{
  mHitBoxBlock[0] = assetLinkerObject().createLinkerBlock("__asset_characterset_hitbox_left");
  mHitBoxBlock[1] = assetLinkerObject().createLinkerBlock("__asset_characterset_hitbox_top");
  mHitBoxBlock[2] = assetLinkerObject().createLinkerBlock("__asset_characterset_hitbox_width");
  mHitBoxBlock[3] = assetLinkerObject().createLinkerBlock("__asset_characterset_hitbox_height");
}

// -----------------------------------------------------------------------------
CharacterAssetSet::~CharacterAssetSet() {
  for (CharacterComponent *component : mComponents) {
    delete component;
  }
}

// -----------------------------------------------------------------------------
void CharacterAssetSet::addComponent(CharacterComponent *comp) {
  mComponents.push_back(comp);
}

// -----------------------------------------------------------------------------
CharacterComponent *CharacterAssetSet::getComponent(const QString &type) const {
  for (CharacterComponent *comp : mComponents) {
    if (comp->typeId() == type) {
      return comp;
    }
  }

  return nullptr;
}

// -----------------------------------------------------------------------------
int CharacterAssetSet::getHitBoxId(const QRect &rect) {
  QString hash = QString::number(rect.left()) + ":" + QString::number(rect.top()) + ":"
    + QString::number(rect.width()) + ":" + QString::number(rect.height());

  if (mHitBoxIndex.contains(hash)) {
    return mHitBoxIndex[hash];
  }

  int id = mHitBoxBlock[0]->getSize();
  mHitBoxBlock[0]->writeNumber(-rect.left(), 2);
  mHitBoxBlock[1]->writeNumber(-rect.top(), 2);
  mHitBoxBlock[2]->writeNumber(rect.width(), 2);
  mHitBoxBlock[3]->writeNumber(rect.height(), 2);
  mHitBoxIndex.insert(hash, id);
  return id;
}
