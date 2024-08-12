#include <QDebug>
#include <aoba/sprite/Sprite.hpp>
#include <aoba/sprite/SpriteType.hpp>
#include <aoba/sprite/SpriteAnimationIndex.hpp>
#include <aoba/character/Character.hpp>
#include <aoba/character/CharacterSpriteComponent.hpp>
#include <aoba/level/Level.hpp>
#include <aoba/level/LevelNpcComponent.hpp>
#include <maker/map/NpcToolWidget.hpp>
#include "ui_NpcToolWidget.h"

// -----------------------------------------------------------------------------
NpcToolWidget::NpcToolWidget(QWidget *parent)
  : QWidget(parent)
  , mUi(new Ui::NpcToolWidget)
{
  mUi->setupUi(this);

  connect(mUi->npcCharacterSelect, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &NpcToolWidget::updateDirectionList);
  connect(mUi->npcActionScript, &ScriptSelectWidget::scriptIdChanged, this, &NpcToolWidget::updateNpc);
  connect(mUi->npcInitializeScript, &ScriptSelectWidget::scriptIdChanged, this, &NpcToolWidget::updateNpc);
  connect(mUi->npcMoveScript, &ScriptSelectWidget::scriptIdChanged, this, &NpcToolWidget::updateNpc);

  setSelectedNpc({});
}

// -----------------------------------------------------------------------------
NpcToolWidget::~NpcToolWidget() {
  delete mUi;
}

// -----------------------------------------------------------------------------
void NpcToolWidget::setSelectedNpc(const QVector<uint32_t> &list) {
  mSelection.clear();

  if (mLevel && mNpcs && !list.isEmpty()) {
    const Aoba::LevelNpc *npc;
    const Aoba::LevelNpcInstance *inst;
    
    if (mNpcs->findNpc(mLevel, list.first(), &npc, &inst)) {
      setSelectedCharacter(npc->mCharacter, inst->mDirection, inst->mFrontLayer);
      mUi->npcActionScript->setScriptId(npc->mScripts.mAction);
      mUi->npcInitializeScript->setScriptId(npc->mScripts.mInitialize);
      mUi->npcMoveScript->setScriptId(npc->mScripts.mMovement);
      mUi->npcCrowdIdEdit->setText(npc->mCrowdId);
    }
  }

  mSelection = list;
  updateUi();
}

// -----------------------------------------------------------------------------
void NpcToolWidget::setSelectedCharacter(const QString &character, const QString &direction, bool front) {
  mUi->npcForegroundCheckBox->setChecked(front);

  int row = mUi->npcCharacterSelect->findData(character);
  if (row < 0) {
    return;
  }

  mUi->npcCharacterSelect->setCurrentIndex(row);
  updateDirectionList();

  mUi->npcDirectionSelect->setCurrentIndex(mUi->npcDirectionSelect->findData(direction));
}

// -----------------------------------------------------------------------------
void NpcToolWidget::setProject(Maker::GameProject *project) {
  mProject = project;
  setLevel(mLevel);
}

// -----------------------------------------------------------------------------
void NpcToolWidget::setLevel(Aoba::Level *level) {
  mLevel = level;

  if (mLevel) {
    mNpcs = mLevel->assetSet()->getComponent<Aoba::LevelNpcComponent>();
  } else {
    mNpcs = nullptr;
  }

  updateCharacterList();
  updateUi();
}

// -----------------------------------------------------------------------------
void NpcToolWidget::updateCharacterList() {
  mUi->npcCharacterSelect->clear();

  if (!mProject || !mLevel || !mNpcs) {
    return;
  }

  for (Aoba::Character *c : mNpcs->characters()->all()) {
    mUi->npcCharacterSelect->addItem(c->name(), c->id());
  }
}

// -----------------------------------------------------------------------------
void NpcToolWidget::updateDirectionList() {
  mUi->npcDirectionSelect->clear();

  Aoba::Character *character = currentCharacter();
  if (!character) {
    return;
  }

  Aoba::CharacterSpriteComponent *spriteComp = character->assetSet()->getComponent<Aoba::CharacterSpriteComponent>();
  if (!spriteComp) {
    return;
  }

  Aoba::Sprite *sprite = spriteComp->getSpriteFor(character);
  if (!sprite) {
    return;
  }

  QMapIterator<QString, int> it(sprite->spriteType()->animationIndex()->directions());
  while (it.hasNext()) {
    it.next();
    mUi->npcDirectionSelect->addItem(it.key(), it.key());
  }
}

// -----------------------------------------------------------------------------
void NpcToolWidget::on_npcCharacterSelect_currentIndexChanged(int) {
  updateNpc();
  emit visualChanged();
}

// -----------------------------------------------------------------------------
void NpcToolWidget::on_npcCrowdIdEdit_textChanged() {
  updateNpc();
}

// -----------------------------------------------------------------------------
void NpcToolWidget::on_npcDirectionSelect_currentIndexChanged(int) {
  updateInstance();
  emit visualChanged();
}

// -----------------------------------------------------------------------------
void NpcToolWidget::on_npcForegroundCheckBox_stateChanged(int) {
  updateInstance();
}

// -----------------------------------------------------------------------------
void NpcToolWidget::updateNpc() {
  int characterIndex = mUi->npcCharacterSelect->currentIndex();
  if (!mLevel || !mNpcs || characterIndex < 0) {
    return;
  }

  const Aoba::LevelNpc *oldNpc;
  const Aoba::LevelNpcInstance *oldInstance;

  Aoba::LevelNpc npc;
  npc.mCharacter = mUi->npcCharacterSelect->itemData(characterIndex).toString();
  npc.mCrowdId = mUi->npcCrowdIdEdit->text();
  npc.mScripts.mAction = mUi->npcActionScript->scriptId();
  npc.mScripts.mMovement = mUi->npcMoveScript->scriptId();
  npc.mScripts.mInitialize = mUi->npcInitializeScript->scriptId();

  for (uint32_t id : mSelection) {
    if (!mNpcs->findNpc(mLevel, id, &oldNpc, &oldInstance)) {
      continue;
    }

    mNpcs->modifyNpc(mLevel, id, npc, *oldInstance);
  }
}

// -----------------------------------------------------------------------------
void NpcToolWidget::updateInstance() {
  int directionIndex = mUi->npcDirectionSelect->currentIndex();
  if (!mLevel || !mNpcs) {
    return;
  }

  QString direction = "";
  if (directionIndex >= 0) {
    direction = mUi->npcDirectionSelect->itemData(directionIndex).toString();
  }

  const Aoba::LevelNpc *oldNpc;
  const Aoba::LevelNpcInstance *oldInstance;

  for (uint32_t id : mSelection) {
    if (!mNpcs->findNpc(mLevel, id, &oldNpc, &oldInstance)) {
      continue;
    }

    Aoba::LevelNpcInstance newInstance(*oldInstance);
    newInstance.mDirection = direction;
    newInstance.mFrontLayer = mUi->npcForegroundCheckBox->isChecked();
    mNpcs->setNpcInstance(mLevel, id, newInstance);
  }
}

// -----------------------------------------------------------------------------
Aoba::Character *NpcToolWidget::currentCharacter() const {
  if (!mProject || !mLevel || !mNpcs) {
    return nullptr;
  }

  int index = mUi->npcCharacterSelect->currentIndex();
  if (index == -1) {
    return nullptr;
  }

  QString id = mUi->npcCharacterSelect->itemData(index).toString();
  return mNpcs->characters()->get(id);
}

// -----------------------------------------------------------------------------
void NpcToolWidget::updateUi() {
  bool isValid = mLevel != nullptr && mProject != nullptr && mNpcs != nullptr;
  bool hasSelection = isValid && !mSelection.isEmpty();

  mUi->npcCharacterLabel->setEnabled(hasSelection);
  mUi->npcCharacterSelect->setEnabled(hasSelection);
  mUi->npcDirectionLabel->setEnabled(hasSelection);
  mUi->npcDirectionSelect->setEnabled(hasSelection);
  mUi->npcForegroundCheckBox->setEnabled(hasSelection);
  mUi->npcActionScript->setEnabled(hasSelection);
  mUi->npcInitializeScript->setEnabled(hasSelection);
  mUi->npcMoveScript->setEnabled(hasSelection);
  mUi->npcCrowdIdEdit->setEnabled(hasSelection);
}