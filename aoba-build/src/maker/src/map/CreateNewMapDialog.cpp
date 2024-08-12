#include <aoba/log/Log.hpp>
#include <QPushButton>
#include <QMessageBox>
#include <aoba/level/LevelPaletteGroupComponent.hpp>
#include <aoba/level/LevelTilemapComponent.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameLevelAssetType.hpp>
#include <maker/game/GamePaletteGroupAssetType.hpp>
#include <maker/game/GameTilemapAssetType.hpp>
#include <maker/game/GameTilesetAssetType.hpp>
#include <maker/project/MakerProject.hpp>
#include <maker/map/CreateNewMapDialog.hpp>
#include <maker/IdValidator.hpp>
#include <maker/GlobalContext.hpp>
#include "ui_CreateNewMapDialog.h"

// -----------------------------------------------------------------------------
CreateNewMapDialog::CreateNewMapDialog(const MakerProjectPtr &project, QWidget *parent)
	: QDialog(parent)
  , mProject(project)
	, mUi(new Ui::CreateNewMapDialog)
{
	mUi->setupUi(this);

  mUi->idEdit->setValidator(new IdValidator(this));

  connect(this->mUi->nameEdit, &QLineEdit::textChanged, this, &CreateNewMapDialog::setIdFromName);
  connect(this->mUi->idEdit, &QLineEdit::textChanged, this, &CreateNewMapDialog::revalidate);

  mLevelTypes = mProject->gameProject()->assetTypes().getAll<Maker::GameLevelAssetType>();
  for (Maker::GameLevelAssetType *level : mLevelTypes) {
    mUi->typeSelect->addItem(level->id(), level->id());
  }

  updateUi();
  revalidate();
}

// -----------------------------------------------------------------------------
CreateNewMapDialog::~CreateNewMapDialog() {
	delete mUi;
	mUi = nullptr;
}

// -----------------------------------------------------------------------------
void CreateNewMapDialog::on_typeSelect_currentIndexChanged(int index) {
  if (index == -1) {
    mValid = false;
    updateUi();
    return;
  }

  QString type = mUi->typeSelect->itemData(index).toString();
  mLevel = dynamic_cast<Aoba::LevelAssetSet*>(mProject->gameProject()->assetTypes().get<Maker::GameLevelAssetType>(type)->assetSet());
  if (!mLevel) {
    mValid = false;
    updateUi();
    return;
  }
  
  Aoba::LevelTilemapComponent *tilemapComponent = mLevel->getComponent<Aoba::LevelTilemapComponent>();
  if (!tilemapComponent) {
    mValid = false;
    updateUi();
    return;
  }

  mTilemap = tilemapComponent->tilemaps();
  if (!mTilemap) {
    mValid = false;
    updateUi();
    return;
  }

  mValid = true;
  mUi->widthSelect->setMaximum(mTilemap->maxWidth());
  mUi->heightSelect->setMaximum(mTilemap->maxHeight());

  updateUi();
  loadTilesets();
}

// -----------------------------------------------------------------------------
void CreateNewMapDialog::loadTilesets() {
  mUi->tilesetSelect->clear();

  if (!mTilemap) {
    return;
  }

  for (Aoba::Tileset *tileset : mTilemap->tileset()->all()) {
    mUi->tilesetSelect->addItem(tileset->name(), tileset->id());
  }
}

// -----------------------------------------------------------------------------
void CreateNewMapDialog::updateUi() {

}

// -----------------------------------------------------------------------------
QString CreateNewMapDialog::id() const {
  return mUi->idEdit->text().trimmed();
}

// -----------------------------------------------------------------------------
QString CreateNewMapDialog::name() const {
  return mUi->nameEdit->text().trimmed();
}

// -----------------------------------------------------------------------------
void CreateNewMapDialog::setIdFromName() {
  if (mUi->idEdit->text() != mAutomatedId || !mLevel) {
    revalidate();
    return;
  }

  QString baseId = name().toLower().replace(QRegExp("[^a-z0-9_]+"), "_").trimmed();
  QString id = baseId;

  int nextId = 2;
  while (mProject->gameProject()->assets().getAsset(mLevel->typeId(), id) != nullptr) {
    id = baseId + QString::number(nextId++);
  }

  mAutomatedId = id;
  mUi->idEdit->setText(id);
  revalidate();
}

// -----------------------------------------------------------------------------
void CreateNewMapDialog::revalidate() {
  bool hasId = !id().isEmpty();
  bool hasName = !name().isEmpty();
  bool hasTileset = mUi->tilesetSelect->currentIndex() != -1;
  bool hasValidId = mLevel && mProject->gameProject()->assets().getAsset(mLevel->typeId(), id()) == nullptr;;
  bool isValid = hasId && hasName && hasTileset && hasValidId && mValid;

  mUi->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(isValid);
}

// -----------------------------------------------------------------------------
QString CreateNewMapDialog::safeTilemapId(const QString &baseId) const {
  if (!mTilemap) {
    return baseId;
  }

  QString id = baseId;
  int nextId = 2;
  while (mProject->gameProject()->assets().getAsset(mTilemap->typeId(), id) != nullptr) {
    id = baseId + QString::number(nextId++);
  }

  return id;
}

// -----------------------------------------------------------------------------
Maker::GameLevelAsset *CreateNewMapDialog::level() {
  if (mResult) {
    return mResult;
  }

  Aoba::PaletteGroupSet *paletteType = mTilemap->tileset()->tiles().front()->paletteGroupSet();

  QSize mapSize(mUi->widthSelect->value(), mUi->heightSelect->value());

  // Reads the tileset
  QString tilesetId = mUi->tilesetSelect->itemData(mUi->tilesetSelect->currentIndex()).toString();
  Maker::GameTilesetAsset *tileset = dynamic_cast<Maker::GameTilesetAsset*>(mProject->gameProject()->assets().getAsset(mTilemap->tileset()->typeId(), tilesetId));
  if (!tileset) {
    Aoba::log::warn("Failed to fetch tileset for some reason...");
    return nullptr;
  }

  Maker::GamePaletteGroupAsset *paletteGroup = dynamic_cast<Maker::GamePaletteGroupAsset*>(mProject->gameProject()->assets().getAsset(paletteType->typeId(), tilesetId + "/main"));
  if (!paletteGroup) {
    Aoba::log::warn("Could not find any palette for the tileset...");
    return nullptr;
  }

  // Creates the tilemap first
  Maker::GameTilemapAssetType *gameTilemaps = mProject->gameProject()->assetTypes().get<Maker::GameTilemapAssetType>(mTilemap->typeId());
  Maker::GameTilemapAsset *gameTilemap = gameTilemaps->create(safeTilemapId(id()));
  Aoba::Tilemap *tilemap = gameTilemap->createNew(tileset->asset(), mapSize);
  tilemap->setName(name());

  // Creates the level itself
  Maker::GameLevelAssetType *gameLevels = mProject->gameProject()->assetTypes().get<Maker::GameLevelAssetType>(mLevel->typeId());
  Maker::GameLevelAsset *gameLevel = gameLevels->create(id());
  Aoba::Level *level = gameLevel->createNew();

  Aoba::LevelTilemapComponent *tilemapComponent = level->assetSet()->getComponent<Aoba::LevelTilemapComponent>();
  if (!tilemapComponent) {
    Aoba::log::warn("Failed to fetch tilemap component");
    return nullptr;
  }
  tilemapComponent->setTilemap(level, tilemap);

  Aoba::LevelPaletteGroupComponent *paletteComponent = level->assetSet()->getComponent<Aoba::LevelPaletteGroupComponent>();
  if (!paletteComponent) {
    Aoba::log::warn("Failed to fetch palette component");
    return nullptr;
  }
  paletteComponent->setPalette(level, paletteGroup->asset());

  if (!tilemap->save() || !level->save()) {
    Aoba::log::warn("Unable to save either tilemap or level");
    return nullptr;
  }

  // Adds the level to the tree
  mProject->assetTree().root().addChild(MakerAssetTreeNode(name(), mLevel->typeId(), id()));
  mProject->gameProject()->assets().add(gameTilemap);
  mProject->gameProject()->assets().add(gameLevel);
  if (!mProject->gameProject()->assets().save()) {
    Aoba::log::warn("Unable to save assets.yml");
    return nullptr;
  }
  if (!mProject->assetTree().saveToProject(mProject->gameProject())) {
    Aoba::log::warn("Unable to save asset tree");
    return nullptr;
  }

  mResult = gameLevel;
  return gameLevel;
}