#include <aoba/log/Log.hpp>
#include <QComboBox>
#include <aoba/tileset/TilesetTile.hpp>
#include <maker/tileset/TilesetEditor.hpp>
#include <maker/tileset/OriginalTilesetScene.hpp>
#include <maker/tileset/TilesetEditorScene.hpp>
#include <maker/project/MakerProject.hpp>
#include <maker/game/GameAsset.hpp>
#include <maker/game/GameTilesetAssetType.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <maker/game/GameProject.hpp>
#include "ui_TilesetEditor.h"



// -----------------------------------------------------------------------------
TilesetEditorTilesetLayer::TilesetEditorTilesetLayer(Aoba::TilesetLayer *layer)
  : mLayer(layer)
{
  Rpg::TilesetCollisionModuleInstance *col = nullptr;
  QMap<const Aoba::TilesetTile*, Aoba::TilesetTile*> copyMap;

  for (Aoba::TilesetAssetSetModule *mod : layer->tileset()->assetSet()->modules()) {
    Aoba::TilesetAssetSetModuleInstance *instance = mod->instanceFor(layer->tileset());
    
    if (instance->typeId() == Rpg::TilesetCollisionModuleInstance::TypeId()) {
      col = dynamic_cast<Rpg::TilesetCollisionModuleInstance*>(instance);
    }
  }
      
  for (const Aoba::TilesetTile *original : mLayer->allTiles()) {
    Aoba::TilesetTile *clone = original->clone();
    copyMap.insert(original, clone);
    mTiles.push_back(clone);

    if (col) {
      mCollisions.insert(clone, col->forTile(original));
    }
  }

  for (Aoba::TilesetAssetSetModule *mod : layer->tileset()->assetSet()->modules()) {
    Aoba::TilesetAssetSetModuleInstance *instance = mod->instanceFor(layer->tileset());
    instance->replaceOldTiles(copyMap);
  }
}

// -----------------------------------------------------------------------------
TilesetEditorTilesetLayer::~TilesetEditorTilesetLayer() {
  for (Aoba::TilesetTile *tile : mTiles) {
    delete tile;
  }
}

// -----------------------------------------------------------------------------
int TilesetEditorTilesetLayer::numTilesPerRow() const {
  return 128 / mLayer->tileset()->assetSet()->tileWidth();
}

// -----------------------------------------------------------------------------
QSize TilesetEditorTilesetLayer::size() const {
  int tilesPerRow = numTilesPerRow();
  int numRows = qMax(1, (mTiles.size() + tilesPerRow - 1) / tilesPerRow);

  return QSize(tilesPerRow, numRows);
}

// -----------------------------------------------------------------------------
void TilesetEditorTilesetLayer::addTileRow() {
  int tilesPerRow = numTilesPerRow();
  for (int i=0; i<tilesPerRow; i++) {
    mTiles.push_back(new Aoba::TilesetTile(mLayer, mTiles.count()));
  }
}

// -----------------------------------------------------------------------------
Aoba::TilesetTile *TilesetEditorTilesetLayer::tileAt(const QPoint &pos) {
  int index = pos.x() + pos.y() * numTilesPerRow();

  if (index < 0) {
    return nullptr;
  }
  
  while (index >= mTiles.size()) {
    mTiles.push_back(new Aoba::TilesetTile(mLayer, mTiles.count()));
  }

  return mTiles[index];
}

// -----------------------------------------------------------------------------
Rpg::TilesetTileCollision TilesetEditorTilesetLayer::collisionFor(Aoba::TilesetTile *tile) const {
  return mCollisions[tile];
}

// -----------------------------------------------------------------------------
Rpg::TilesetTileCollision TilesetEditorTilesetLayer::collisionFor(const QPoint &pos) {
  Aoba::TilesetTile *tile = tileAt(pos);
  if (tile == nullptr) {
    return Rpg::TilesetTileCollision();
  }

  return mCollisions[tile];
}

// -----------------------------------------------------------------------------
Aoba::TilesetTile *TilesetEditorTilesetLayer::requireTileAt(const QPoint &pos) {
  int index = pos.x() + pos.y() * numTilesPerRow();

  while (index >= mTiles.size()) {
    mTiles.push_back(new Aoba::TilesetTile(mLayer, index));
  }

  return mTiles[index];
}

// -----------------------------------------------------------------------------
Rpg::TilesetTileCollision &TilesetEditorTilesetLayer::requestCollisionFor(const QPoint &pos) {
  return mCollisions[requireTileAt(pos)];
}





// -----------------------------------------------------------------------------
TilesetEditorTileset::TilesetEditorTileset(Aoba::Tileset *tileset)
  : mTileset(tileset)
{
  for (Aoba::TilesetAssetSetLayer *layerConfig : tileset->assetSet()->layer()) {
    layer(tileset->layer(layerConfig));
  }
}

// -----------------------------------------------------------------------------
TilesetEditorTileset::~TilesetEditorTileset() {
  for (TilesetEditorTilesetLayer *layer : mLayers) {
    delete layer;
  }
}

// -----------------------------------------------------------------------------
TilesetEditorTilesetLayer *TilesetEditorTileset::layer(Aoba::TilesetLayer *layer) {
  if (!mLayers.contains(layer)) {
    mLayers.insert(layer, new TilesetEditorTilesetLayer(layer));
  }

  return mLayers[layer];
}



// -----------------------------------------------------------------------------
bool TilesetEditorTool::requiresCollisionLayer() const {
  switch (mTool) {
    case COLLISION_EDGE:
    case MOVE_EFFECT:
    case ENTER_EVENT:
    case EXIT_FLAG:
    case COLLISION_GROUP:
      return true;

    case PRIORITY_EDGE:
    case GRAPHIC:
    case INVALID:
      return false;
  }
  return false;
}

// -----------------------------------------------------------------------------
bool TilesetEditorTool::editBgTiles() const {
  switch (mTool) {
    case PRIORITY_EDGE:
    case COLLISION_EDGE:
      return true;

    case MOVE_EFFECT:
    case ENTER_EVENT:
    case EXIT_FLAG:
    case GRAPHIC:
    case COLLISION_GROUP:
    case INVALID:
      return false;
  }
  return false;
}

// -----------------------------------------------------------------------------
void TilesetEditorPriorityTool::startEdit(TilesetEditorTilesetLayer *layer, const QRect &rect) {
  QSize numBgTiles(layer->layer()->tileset()->assetSet()->numBgTilesPerTile());
  Aoba::TilesetTile *tile = layer->requireTileAt(QPoint(rect.topLeft().x() / numBgTiles.width(), rect.topLeft().y() / numBgTiles.height()));

  mOldValue = tile->tileLayer(mTileLayer)->constPart(rect.topLeft().x() % numBgTiles.width(), rect.topLeft().y() % numBgTiles.height()).priority();
}

// -----------------------------------------------------------------------------
void TilesetEditorPriorityTool::triggerEdit(TilesetEditorTilesetLayer *layer, const QRect &rect) {
  QSize numBgTiles(layer->layer()->tileset()->assetSet()->numBgTilesPerTile());
  Aoba::TilesetTile *tile = layer->requireTileAt(QPoint(rect.topLeft().x() / numBgTiles.width(), rect.topLeft().y() / numBgTiles.height()));
  tile->tileLayer(mTileLayer)->part(rect.topLeft().x() % numBgTiles.width(), rect.topLeft().y() % numBgTiles.height()).setPriority(!mOldValue);
}

// -----------------------------------------------------------------------------
void TilesetEditorCollisionEdgeTool::startEdit(TilesetEditorTilesetLayer *layer, const QRect &rect) {
  const Rpg::TilesetTileCollision &col = layer->requestCollisionFor(QPoint(rect.topLeft().x() / 2, rect.topLeft().y() / 2));
  int edge = (rect.topLeft().x() % 2) + (rect.topLeft().y() % 2) * 2;
  mOldValue = col.collision[edge];
}

// -----------------------------------------------------------------------------
void TilesetEditorCollisionEdgeTool::triggerEdit(TilesetEditorTilesetLayer *layer, const QRect &rect) {
  Rpg::TilesetTileCollision &col = layer->requestCollisionFor(QPoint(rect.topLeft().x() / 2, rect.topLeft().y() / 2));
  int edge = (rect.topLeft().x() % 2) + (rect.topLeft().y() % 2) * 2;
  col.setCollision(edge, !mOldValue);
}

// -----------------------------------------------------------------------------
void TilesetEditorCollisionGroupTool::triggerEdit(TilesetEditorTilesetLayer *layer, const QRect &rect) {
  layer->requestCollisionFor(QPoint(rect.topLeft().x(), rect.topLeft().y())).layer[0] = mId & 1;
  layer->requestCollisionFor(QPoint(rect.topLeft().x(), rect.topLeft().y())).layer[1] = mId & 2;
}

// -----------------------------------------------------------------------------
void TilesetEditorEnterEventTool::triggerEdit(TilesetEditorTilesetLayer *layer, const QRect &rect) {
  layer->requestCollisionFor(QPoint(rect.topLeft().x(), rect.topLeft().y())).setEvent((Rpg::TilesetTileCollision::EnterEffect)mId);
}

// -----------------------------------------------------------------------------
void TilesetEditorMoveEffectTool::triggerEdit(TilesetEditorTilesetLayer *layer, const QRect &rect) {
  layer->requestCollisionFor(QPoint(rect.topLeft().x(), rect.topLeft().y())).setMove((Rpg::TilesetTileCollision::MoveEffect)mId);
}

// -----------------------------------------------------------------------------
void TilesetEditorExitTool::startEdit(TilesetEditorTilesetLayer *layer, const QRect &rect) {
  mOldValue = layer->requestCollisionFor(QPoint(rect.topLeft().x(), rect.topLeft().y())).exit;
}

// -----------------------------------------------------------------------------
void TilesetEditorExitTool::triggerEdit(TilesetEditorTilesetLayer *layer, const QRect &rect) {
  layer->requestCollisionFor(QPoint(rect.topLeft().x(), rect.topLeft().y())).exit = !mOldValue;
}

// -----------------------------------------------------------------------------
void TilesetEditorGraphicTool::triggerEdit(TilesetEditorTilesetLayer *layer, const QRect &rect) {
  QRect finalRect = rect.intersected(QRect(QPoint(0, 0), layer->size()));
  QRect brush = mEditor->selectedTiles();
  if (!brush.isValid()) {
    return;
  }

  Aoba::Tileset *tileset = layer->layer()->tileset();
  Aoba::TilesetAssetSet *tilesetConfig = tileset->assetSet();
  Aoba::TilesetTiles *tiles = tileset->tiles(layer->layer()->config()->tiles());

  int numBgTilesX = tilesetConfig->numBgTilesX();
  int numBgTilesY = tilesetConfig->numBgTilesY();
  int numBgTilesPerRow = tiles->numBgTilesX();

  for (int y=finalRect.top(); y<=finalRect.bottom(); y++) {
    for (int x=finalRect.left(); x<=finalRect.right(); x++) {
      int bx = brush.left() + (x - finalRect.left()) % brush.width();
      int by = brush.top() + (y - finalRect.top()) % brush.height();

      for (int ey=0; ey<numBgTilesY; ey++) {
        for (int ex=0; ex<numBgTilesX; ex++) {
          int index = (bx * numBgTilesX + ex) + (by * numBgTilesY + ey) * numBgTilesPerRow;

          layer->tileAt(QPoint(x, y))->tileLayer(mTileLayer)->part(ex, ey).setBgTileIndex(index);
        }
      }
    }
  }
}




// -----------------------------------------------------------------------------
TilesetEditor::TilesetEditor(const MakerProjectPtr &project, QWidget *parent)
  : QMainWindow(parent)
  , mUi(new Ui::TilesetEditor)
  , mProject(project)
  , mTool(new TilesetEditorPriorityTool())
  , mEditorScene(new TilesetEditorScene(this))
  , mTilesScene(new OriginalTilesetScene(this))
{
  mUi->setupUi(this);

  mUi->actionGrid->setChecked(true);

  mUi->editorView->setMouseTracking(true);
  mUi->editorView->setScene(mEditorScene);
  mUi->editorView->scale(3, 3);

  mUi->tileSelectView->setScene(mTilesScene);
  mUi->tileSelectView->scale(2, 2);

  connect(mTilesScene, &OriginalTilesetScene::selectionChanged, this, &TilesetEditor::onGraphicsClicked);

  mEnterEvents = {
    mUi->toolEnter0,
    nullptr,
    mUi->toolEnter2,
    mUi->toolEnter3,
    mUi->toolEnter4,
    mUi->toolEnter5,
    mUi->toolEnter6,
    mUi->toolEnter7
  };
  mMoveEffects = {
    mUi->toolMove0,
    mUi->toolMove1,
    mUi->toolMove2,
    mUi->toolMove3,
    mUi->toolMove4,
    nullptr,
    mUi->toolMove6,
    nullptr,
    mUi->toolMove8,
    mUi->toolMove9,
    mUi->toolMove10,
    mUi->toolMove11
  };

  mTilesetList = new QComboBox();
  mLayerList = new QComboBox();

  mTileLayerSelect = new QTabBar(this);
  mUi->targetLayerWidget->layout()->addWidget(mTileLayerSelect);
  mUi->tilesetToolBar->addWidget(mTilesetList);
  mUi->layerToolBar->addWidget(mLayerList);
  mUi->layerToolBar->addAction(mUi->actionAddTiles);

  connect(mTileLayerSelect, SIGNAL(currentChanged(int)), this, SLOT(setCurrentLayerBg(int)));

  connect(mTilesetList, SIGNAL(currentIndexChanged(int)), this, SLOT(onTilesetListCurrentIndexChanged(int)));
  connect(mLayerList, SIGNAL(currentIndexChanged(int)), this, SLOT(onLayerListCurrentIndexChanged(int)));

  connect(mEditorScene, &TilesetEditorScene::editStarted, this, &TilesetEditor::startEdit);
  connect(mEditorScene, &TilesetEditorScene::editTriggered, this, &TilesetEditor::triggerEdit);
  connect(mEditorScene, &TilesetEditorScene::editFinished, this, &TilesetEditor::finishEdit);

  setWindowFlags(Qt::Widget);
  updateTilesetList();

  on_toolPriority_clicked();
}

// -----------------------------------------------------------------------------
TilesetEditor::~TilesetEditor() {
  for (TilesetEditorTileset *tileset : mLoadedTilesets) {
    delete tileset;
  }

  delete mTool;
  delete mLayerList;
  delete mTilesetList;
  delete mUi;
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_actionGrid_triggered() {
  mEditorScene->setGridVisible(mUi->actionGrid->isChecked());
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_actionAddTiles_triggered() {
  if (!mTileset || !mLayer) {
    return;
  }

  mLayer->addTileRow();
  mEditorScene->setLayer(mLayer);
}

// -----------------------------------------------------------------------------
QRect TilesetEditor::selectedTiles() const {
  return mTilesScene->selection();
}

// -----------------------------------------------------------------------------
void TilesetEditor::startEdit(const QRect &rect) {
  if (!mLayer || !rect.isValid()) {
    return;
  }

  mTool->startEdit(mLayer, rect);
  mTileset->setIsModified();
  mEditorScene->redrawLayer();
}

// -----------------------------------------------------------------------------
void TilesetEditor::triggerEdit(const QRect &rect) {
  if (!mLayer || !rect.isValid()) {
    return;
  }

  mTool->triggerEdit(mLayer, rect);
  mTileset->setIsModified();
  mEditorScene->redrawLayer();
}

// -----------------------------------------------------------------------------
void TilesetEditor::finishEdit() {
  if (!mLayer) {
    return;
  }

  mTool->finishEdit(mLayer);
  mTileset->setIsModified();
  mEditorScene->redrawLayer();
}

// -----------------------------------------------------------------------------
void TilesetEditor::setCurrentLayerBg(int index) {
  if (mTool) {
    mTool->setCurrentTileLayer(index);
  }
}

// -----------------------------------------------------------------------------
void TilesetEditor::updateTilesetList() {
  mTilesetList->clear();

  mTilesets = mProject->gameProject()->assetTypes().getAll<Maker::GameTilesetAssetType>();
  if (mTilesets.isEmpty()) {
    updateUi();
    return;
  }

  for (Maker::GameTilesetAssetType *tilesetType : mTilesets) {
    for (const Aoba::Tileset *tileset : tilesetType->assetSet()->all()) {
      if (tileset->isGeneratedTileset()) {
        continue;
      }

      mTilesetList->addItem(tileset->name(), tilesetType->id() + "/" + tileset->id());
    }
  }

  mTilesetList->setCurrentIndex(0);
  onTilesetListCurrentIndexChanged(mTilesetList->currentIndex());
  updateUi();
}

// -----------------------------------------------------------------------------
void TilesetEditor::onTilesetListCurrentIndexChanged(int index) {
  mTileset = nullptr;
  mLayer = nullptr;
  mEditorScene->setLayer(nullptr);

  if (index == -1 || mTilesets.isEmpty()) {
    updateUi();
    return;
  }

  QStringList part = mTilesetList->itemData(index).toString().split('/');
  QString typeId = part[0];
  QString id = part[1];
  Maker::GameTilesetAsset *gameTileset = dynamic_cast<Maker::GameTilesetAsset*>(mProject->gameProject()->assets().getAsset(typeId, id));
  if (!gameTileset) {
    updateLayerList();
    return;
  }

  Aoba::Tileset *tileset = gameTileset->asset();
  if (!mLoadedTilesets.contains(tileset->id())) {
    mLoadedTilesets.insert(tileset->id(), new TilesetEditorTileset(tileset));
  }

  mTileset = mLoadedTilesets[tileset->id()];

  testToolIsValid();
  updateLayerList();
  updateUi();
  updateUiTool();
}

// -----------------------------------------------------------------------------
void TilesetEditor::updateLayerList() {
  mLayerList->clear();

  if (!mTileset) {
    return;
  }

  for (const auto *layerConfig : mTileset->tileset()->assetSet()->layer()) {
    mLayerList->addItem(layerConfig->id(), layerConfig->id());
  }

  mLayerList->setCurrentIndex(0);
  onLayerListCurrentIndexChanged(mLayerList->currentIndex());
}

// -----------------------------------------------------------------------------
void TilesetEditor::onLayerListCurrentIndexChanged(int index) {
  mLayer = nullptr;
  mEditorScene->setLayer(nullptr);

  if (index == -1 || !mTileset) {
    updateUi();
    return;
  }

  QString layerId = mLayerList->itemData(mLayerList->currentIndex()).toString();
  Aoba::TilesetLayer *layer = mTileset->tileset()->layer(mTileset->tileset()->assetSet()->getLayer(layerId));
  if (layer == nullptr) {
    updateUi();
    return;
  }

  mLayer = mTileset->layer(layer);
  mEditorScene->setLayer(mLayer);
  mTilesScene->setLayer(layer->tileset()->tiles(layer->config()->tiles()));
  mUi->tileSelectView->setMinimumHeight(mTilesScene->sceneRect().height() * 2 + 16);

  while (mTileLayerSelect->count() > 0) {
    mTileLayerSelect->removeTab(0);
  }
  if (mLayer->layer()->config()->numBackgrounds() > 1) {
    for (int i=0; i<mLayer->layer()->config()->numBackgrounds(); i++) {
      mTileLayerSelect->addTab(tr("BG %1").arg(i + 1));
    }
  }

  updateUi();
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolPriority_clicked() {
  setTool(new TilesetEditorPriorityTool());
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolCollisionType0_clicked() {
  setTool(new TilesetEditorCollisionGroupTool(0));
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolCollisionType1_clicked() {
  setTool(new TilesetEditorCollisionGroupTool(1));
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolCollisionType2_clicked() {
  setTool(new TilesetEditorCollisionGroupTool(2));
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolCollisionType3_clicked() {
  setTool(new TilesetEditorCollisionGroupTool(3));
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolCollision_clicked() {
  setTool(new TilesetEditorCollisionEdgeTool());
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolExit_clicked() {
  setTool(new TilesetEditorExitTool());
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolEnter0_clicked() {
  setTool(new TilesetEditorEnterEventTool(0));
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolEnter2_clicked() {
  setTool(new TilesetEditorEnterEventTool(2));
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolEnter3_clicked() {
  setTool(new TilesetEditorEnterEventTool(3));
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolEnter4_clicked() {
  setTool(new TilesetEditorEnterEventTool(4));
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolEnter5_clicked() {
  setTool(new TilesetEditorEnterEventTool(5));
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolEnter6_clicked() {
  setTool(new TilesetEditorEnterEventTool(6));
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolEnter7_clicked() {
  setTool(new TilesetEditorEnterEventTool(7));
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolMove0_clicked() {
  setTool(new TilesetEditorMoveEffectTool(0));
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolMove1_clicked() {
  setTool(new TilesetEditorMoveEffectTool(1));
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolMove2_clicked() {
  setTool(new TilesetEditorMoveEffectTool(2));
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolMove3_clicked() {
  setTool(new TilesetEditorMoveEffectTool(3));
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolMove4_clicked() {
  setTool(new TilesetEditorMoveEffectTool(4));
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolMove6_clicked() {
  setTool(new TilesetEditorMoveEffectTool(6));
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolMove8_clicked() {
  setTool(new TilesetEditorMoveEffectTool(8));
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolMove9_clicked() {
  setTool(new TilesetEditorMoveEffectTool(9));
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolMove10_clicked() {
  setTool(new TilesetEditorMoveEffectTool(10));
}

// -----------------------------------------------------------------------------
void TilesetEditor::on_toolMove11_clicked() {
  setTool(new TilesetEditorMoveEffectTool(11));
}

// -----------------------------------------------------------------------------
void TilesetEditor::onGraphicsClicked() {
  if (!mTilesScene->selection().isValid()) {
    if (mTool->tool() == TilesetEditorTool::GRAPHIC) {
      setTool(new TilesetEditorGraphicTool(this));
    }
    return;
  }

  setTool(new TilesetEditorGraphicTool(this));
}

// -----------------------------------------------------------------------------
void TilesetEditor::setTool(TilesetEditorTool *tool) {
  delete mTool;
  if (tool) {
    tool->setCurrentTileLayer(mTileLayerSelect->currentIndex());
  }
  mTool = tool;
  testToolIsValid();
  updateUiTool();
}

// -----------------------------------------------------------------------------
bool TilesetEditor::hasCollisionLayer() const {
  if (!mTileset) {
    return false;
  }

  for (auto *mod : mTileset->tileset()->assetSet()->modules()) {
    if (mod->instanceFor(mTileset->tileset())->typeId() == Rpg::TilesetCollisionModuleInstance::TypeId()) {
      return true;
    }
  }
  
  return false;
}

// -----------------------------------------------------------------------------
void TilesetEditor::testToolIsValid() {
  if (!mTileset || !mTool) {
    return;
  }

  if (mTool->requiresCollisionLayer() && !hasCollisionLayer()) {
    setTool(new TilesetEditorPriorityTool());
  }
}

// -----------------------------------------------------------------------------
void TilesetEditor::updateUiTool() {
  bool isCollisionEdge = mTool->tool() == TilesetEditorTool::COLLISION_EDGE;
  bool isCollisionLayer = mTool->tool() == TilesetEditorTool::COLLISION_GROUP;
  bool isDiagonalCollision = mTool->tool() == TilesetEditorTool::MOVE_EFFECT && mTool->id() >= 8;

  mUi->toolExit->setChecked(mTool->tool() == TilesetEditorTool::EXIT_FLAG);
  mUi->toolPriority->setChecked(mTool->tool() == TilesetEditorTool::PRIORITY_EDGE);
  mUi->toolCollision->setChecked(isCollisionEdge);
  mUi->toolCollisionType0->setChecked(isCollisionLayer && mTool->id() == 0);
  mUi->toolCollisionType1->setChecked(isCollisionLayer && mTool->id() == 1);
  mUi->toolCollisionType2->setChecked(isCollisionLayer && mTool->id() == 2);
  mUi->toolCollisionType3->setChecked(isCollisionLayer && mTool->id() == 3);
  for (QToolButton *button : mEnterEvents) {
    if (button) {
      button->setChecked(mTool->tool() == TilesetEditorTool::ENTER_EVENT && mTool->id() == mEnterEvents.indexOf(button));
    }
  }
  for (QToolButton *button : mMoveEffects) {
    if (button) {
      button->setChecked(mTool->tool() == TilesetEditorTool::MOVE_EFFECT && mTool->id() == mMoveEffects.indexOf(button));
    }
  }

  if (mTool->tool() != TilesetEditorTool::GRAPHIC) {
    mTilesScene->setSelection(QRect());
  }

  mEditorScene->setPriorityVisible(mTool->tool() == TilesetEditorTool::PRIORITY_EDGE);
  mEditorScene->setCollisionVisible(isCollisionEdge || isCollisionLayer || isDiagonalCollision);
  mEditorScene->setMovementVisible(mTool->tool() == TilesetEditorTool::MOVE_EFFECT && !isDiagonalCollision);
  mEditorScene->setEventVisible(mTool->tool() == TilesetEditorTool::ENTER_EVENT);
  mEditorScene->setExitVisible(mTool->tool() == TilesetEditorTool::EXIT_FLAG);

  int gridSize = mTool->editBgTiles() ? mEditorScene->bgTileSize().width() : mEditorScene->tileSize().width();
  mEditorScene->setToolGrid(gridSize);
  if (mTool->tool() == TilesetEditorTool::GRAPHIC) {
    mEditorScene->setBrushSize(mTilesScene->selection().size());
  } else {
    mEditorScene->setBrushSize(QSize(1, 1));
  }
}

// -----------------------------------------------------------------------------
void TilesetEditor::updateUi() {
  bool hasTileset = mTileset != nullptr;
  bool hasLayer = mLayer != nullptr;
  bool toolEnabled = hasTileset && hasLayer;
  bool collisionEnabled = hasCollisionLayer();

  mUi->collisionGroup1->setVisible(collisionEnabled);
  mUi->collisionGroup2->setVisible(collisionEnabled);
  mUi->collisionGroup3->setVisible(collisionEnabled);

  if (mTool->tool() != TilesetEditorTool::GRAPHIC) {
    mTilesScene->setSelection(QRect());
  }

  mUi->actionAddTiles->setEnabled(hasLayer);
  mUi->toolExit->setEnabled(toolEnabled);
  mUi->toolPriority->setEnabled(toolEnabled);
  mUi->toolCollision->setEnabled(toolEnabled);
  mUi->toolCollisionType0->setEnabled(toolEnabled);
  mUi->toolCollisionType1->setEnabled(toolEnabled);
  mUi->toolCollisionType2->setEnabled(toolEnabled);
  mUi->toolCollisionType3->setEnabled(toolEnabled);
  for (QToolButton *button : mEnterEvents) {
    if (button) { button->setEnabled(toolEnabled); }
  }
  for (QToolButton *button : mMoveEffects) {
    if (button) { button->setEnabled(toolEnabled); }
  }
}

// -----------------------------------------------------------------------------
bool TilesetEditor::save() const {
  bool ok = true;
  QMapIterator<QString, TilesetEditorTileset*> it(mLoadedTilesets);
  while (it.hasNext()) {
    it.next();
    
    if (it.value()->isModified() && !save(it.value())) {
      ok = false;
    }
  }

  return ok;
}

// -----------------------------------------------------------------------------
bool TilesetEditor::save(TilesetEditorTileset *ourTileset) const {
  Aoba::Tileset *tileset = ourTileset->tileset();
  Rpg::TilesetCollisionModuleInstance *col = nullptr;
  QMap<const Aoba::TilesetTile*, Aoba::TilesetTile*> copyMap;

  for (Aoba::TilesetAssetSetModule *mod : tileset->assetSet()->modules()) {
    Aoba::TilesetAssetSetModuleInstance *inst = mod->instanceFor(tileset);
    if (inst->typeId() == Rpg::TilesetCollisionModuleInstance::TypeId()) {
      col = dynamic_cast<Rpg::TilesetCollisionModuleInstance*>(inst);
      col->clear();
    }
  }

  for (auto *layerConfig : tileset->assetSet()->layer()) {
    Aoba::TilesetLayer *layer = tileset->layer(layerConfig);
    TilesetEditorTilesetLayer *ourLayer = ourTileset->layer(layer);

    layer->clear();
    for (Aoba::TilesetTile *ourTile : ourLayer->tiles()) {
      Aoba::TilesetTile *tile = layer->createTile(ourTile);
      copyMap.insert(ourTile, tile);

      if (col) {
        col->addTile(tile, ourLayer->collisionFor(ourTile));
      }
    }
  }

  for (Aoba::TilesetAssetSetModule *mod : tileset->assetSet()->modules()) {
    Aoba::TilesetAssetSetModuleInstance *inst = mod->instanceFor(tileset);
    inst->replaceOldTiles(copyMap);
  }
  
  return tileset->save();
}