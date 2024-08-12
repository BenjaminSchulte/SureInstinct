#include <aoba/log/Log.hpp>
#include <QCloseEvent>
#include <QTimer>
#include <QTreeView>
#include <QUndoStack>
#include <QMessageBox>
#include <QShortcut>
#include <maker/project/MakerProject.hpp>
#include <maker/project/MakerAssetTree.hpp>
#include <maker/game/GameLevelAssetType.hpp>
#include <maker/game/GameTilemapAssetType.hpp>
#include <maker/game/GameTilesetAssetType.hpp>
#include <maker/common/AobaGraphicsView.hpp>
#include <maker/tileset/TilesetScene.hpp>
#include <maker/tileset/TilesetEditorDialog.hpp>
#include <maker/map/MapLayerList.hpp>
#include <maker/map/MapSceneLayerList.hpp>
#include <maker/map/MapSceneLayer.hpp>
#include <maker/map/ResizeMapDialog.hpp>
#include <maker/map/MapScene.hpp>
#include <maker/map/CreateNewMapDialog.hpp>
#include <maker/map/NpcMapSceneLayer.hpp>
#include <maker/sprite/SpriteEditorDialog.hpp>
#include <maker/MainWindow.hpp>
#include <maker/GlobalContext.hpp>
#include <maker/map/MapEditorContext.hpp>
#include <maker/globals.hpp>
#include <aoba/tilemap/TilemapTerrainLayer.hpp>
#include <aoba/tilemap/Tilemap.hpp>
#include <aoba/tileset/TilesetTerrain.hpp>
#include <aoba/tileset/Tileset.hpp>
#include "ui_MainWindow.h"

// -----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, mUi(new Ui::MainWindow)
	, mContext(new MapEditorContext(this))
	, mTilesetScene(new TilesetScene(mContext->tilesetReader(), this))
{
	mUi->setupUi(this);

	mUi->tilesetGraphics->setScene(mTilesetScene);
	mUi->tilesetGraphics->scale(2, 2);
	setCurrentTool(mUi->tilesetTool);

	connect(mTilesetScene, &TilesetScene::selectionChanged, mContext, &MapEditorContext::setTileBrush);
	connect(mContext, &MapEditorContext::layerListChanged, mUi->layers, &MapLayerList::setLayerList);
	connect(mContext, &MapEditorContext::editLayerChanged, mUi->layers, &MapLayerList::setSelectedLayer);
	connect(mContext, &MapEditorContext::editLayerChanged, this, &MainWindow::setSelectedLayer);
	connect(mContext, &MapEditorContext::tilemapLayerChanged, this, &MainWindow::setSelectedTilemapLayer);
	connect(mContext, &MapEditorContext::terrainLayerChanged, this, &MainWindow::setSelectedTerrainLayer);
	connect(mContext, &MapEditorContext::toolChanged, this, &MainWindow::onEditToolChanged);
	connect(mUi->layers, &MapLayerList::layerChanged, mContext, &MapEditorContext::setEditLayer);
	connect(mUi->terrainListWidget, &MapTerrainLayerList::terrainChanged, mContext, &MapEditorContext::setTerrainBrush);

	connect(new QShortcut(QKeySequence("A"), this), &QShortcut::activated, [this](){ moveTileSelection(-1, 0); });
	connect(new QShortcut(QKeySequence("W"), this), &QShortcut::activated, [this](){ moveTileSelection(0, -1); });
	connect(new QShortcut(QKeySequence("S"), this), &QShortcut::activated, [this](){ moveTileSelection(0, 1); });
	connect(new QShortcut(QKeySequence("D"), this), &QShortcut::activated, [this](){ moveTileSelection(1, 0); });

	mUi->actionToggleGrid->setChecked(true);
	onEditToolChanged();
	updateUi();
}

// -----------------------------------------------------------------------------
MainWindow::~MainWindow() {
	closeProject();
	delete mUi;
	mUi = nullptr;
}

// -----------------------------------------------------------------------------
void MainWindow::setCurrentTool(QWidget *widget) {
	mUi->paletteToolContainer->setCurrentWidget(widget);
	
	if (widget == mUi->terrainTool) {
		mUi->toolPaletteDock->setWindowTitle(tr("Terrain"));
	} else if (widget == mUi->tilesetTool) {
		mUi->toolPaletteDock->setWindowTitle(tr("Tileset"));
	} else if (widget == mUi->currentNpc) {
		mUi->toolPaletteDock->setWindowTitle(tr("Entity"));
	}
}

// -----------------------------------------------------------------------------
void MainWindow::moveTileSelection(int x, int y) {
	QWidget *widget = mUi->paletteToolContainer->currentWidget();
	
	if (widget == mUi->terrainTool) {
		mUi->terrainListWidget->setCurrentRow(qMin(mUi->terrainListWidget->count() - 1, qMax(0, mUi->terrainListWidget->currentRow() + y)));
	} else if (widget == mUi->tilesetTool) {
		mTilesetScene->selectTile(x, y);
	}
}

// -----------------------------------------------------------------------------
void MainWindow::on_actionProjectClose_triggered() {
	tryCloseProject();
}

// -----------------------------------------------------------------------------
void MainWindow::closeProject() {
	if (currentScene()) {
		context().localConfig().setActiveAssetId(currentScene()->level()->abstractType()->id() + "/" + currentScene()->level()->id());
	}

	context().localConfig().setLoadedAssetIds(mViews.keys());
	context().localConfig().save();

	mContext->close();
	mUi->currentNpc->setProject(nullptr);
	mUi->tabMap->clear();
	for (QGraphicsView *view : mViews) {
		QGraphicsScene *scene = view->scene();
		delete view;
		delete scene;
	}
	mViews.clear();
	mCurrentUndoStack = nullptr;

	context().setProject(MakerProjectPtr());
	setWindowTitle(QApplication::applicationDisplayName());
	mUi->projectTree->regenerate();

	updateUi();
}

// -----------------------------------------------------------------------------
void MainWindow::redrawScenes() {
	for (QGraphicsView *view : mViews) {
		dynamic_cast<MapScene*>(view->scene())->redrawTilemap();
	}
}

// -----------------------------------------------------------------------------
void MainWindow::on_newMapButton_clicked() {
	CreateNewMapDialog dialog(context().project(), this);
	if (!dialog.exec()) {
		return;
	}

	if (dialog.level() == nullptr) {
		QMessageBox::critical(this, tr("Create new map"), tr("Could not create map. Check log file for further information."));
		return;
	}

	mUi->projectTree->regenerate();
	openViewById(dialog.level()->asset()->assetSet()->typeId() + "/" + dialog.level()->id());
}

// -----------------------------------------------------------------------------
void MainWindow::loadLastProject() {
	QString projectId = mSettings.value(CURRENT_PROJECT_KEY, QString()).toString();
  loadProject(projectId);
}

// -----------------------------------------------------------------------------
void MainWindow::loadProject(const QString &folderName) {
  MakerProjectPtr project = MakerProject::load(folderName);

	if (!project) {
		QMessageBox::critical(this, tr("Load project"), tr("Failed to load project %1").arg(folderName));
		return;
	}

	context().setProject(project);

	setWindowTitle(QApplication::applicationDisplayName() + " - " + project->title());
	mUi->projectTree->regenerate();

	for (const QString &assetId : context().localConfig().loadedAssetIds()) {
		getView(assetId);
	}

	if (!context().localConfig().activeAssetId().isEmpty()) {
		openViewById(context().localConfig().activeAssetId());
	}

	mUi->currentNpc->setProject(context().project()->gameProject());
	updateUi();
}

// -----------------------------------------------------------------------------
void MainWindow::onHoverInfo(const MapSceneHoverInfo &info) {
	if (!info.valid()) {
		mUi->statusBar->showMessage("");
		return;
	}
	
	mUi->statusBar->showMessage(QString("%1, %2").arg(info.position().x()).arg(info.position().y()));
}

// -----------------------------------------------------------------------------
void MainWindow::onMapSceneSelectionChanged() {
	MapScene *scene = currentScene();
	if (!scene || !mContext->editLayer() || !mContext->editLayer()->isObjectLayer()) {
		resetAllSelections();
		return;
	}

	MapSceneObjectLayer *objectLayer = dynamic_cast<MapSceneObjectLayer*>(mContext->editLayer());
	if (!objectLayer) {
		return;
	}

	QList<QGraphicsItem*> selected(scene->selectedItems());
	if (objectLayer->objectType() == Aoba::LevelNpcComponent::TypeId()) {
		QVector<uint32_t> npcs;
		for (auto *item : selected) {
			GraphicsNpcItem *npc = dynamic_cast<GraphicsNpcItem*>(item);
			if (item) { npcs.push_back(npc->npcId()); }
		}
		mUi->currentNpc->setSelectedNpc(npcs);
	}
}

// -----------------------------------------------------------------------------
void MainWindow::resetAllSelections() {
	mUi->currentNpc->setSelectedNpc({});
}

// -----------------------------------------------------------------------------
void MainWindow::on_actionMirrorX_triggered() {
	mContext->setBrushFlip(mUi->actionMirrorX->isChecked(), mUi->actionMirrorY->isChecked());
}

// -----------------------------------------------------------------------------
void MainWindow::on_actionMirrorY_triggered() {
	mContext->setBrushFlip(mUi->actionMirrorX->isChecked(), mUi->actionMirrorY->isChecked());
}

// -----------------------------------------------------------------------------
void MainWindow::on_actionResizeMap_triggered() {
	MapScene *scene = currentScene();
	if (!scene) {
		return;
	}

	ResizeMapDialog dialog(scene->level(), this);
	if (!dialog.exec()) {
		return;
	}

	Aoba::Tilemap *tilemap = scene->tilemap()->asset();
	scene->level()->asset()->resize(
		dialog.mapOffset().x(),
		dialog.mapOffset().y(),
		dialog.newMapSize().width() - dialog.mapOffset().x() - tilemap->width(),
		dialog.newMapSize().height() - dialog.mapOffset().y() - tilemap->height(),
		dialog.newMapSize()
	);
	Aoba::log::debug("TODO: Remove undo stack");
	on_tabMap_currentChanged(mUi->tabMap->currentIndex());
}

// -----------------------------------------------------------------------------
void MainWindow::on_showTilesetGridButton_clicked() {
	mTilesetScene->setGridVisible(mUi->showTilesetGridButton->isChecked());
}

// -----------------------------------------------------------------------------
void MainWindow::on_showTilesetCollisionButton_clicked() {
	mTilesetScene->setCollisionVisible(mUi->showTilesetCollisionButton->isChecked());
}

// -----------------------------------------------------------------------------
void MainWindow::on_showTilesetEnterButton_clicked() {
	mTilesetScene->setEnterVisible(mUi->showTilesetEnterButton->isChecked());
}

// -----------------------------------------------------------------------------
void MainWindow::on_showTilesetMoveButton_clicked() {
	mTilesetScene->setMovementVisible(mUi->showTilesetMoveButton->isChecked());
}

// -----------------------------------------------------------------------------
void MainWindow::on_showTilesetExitButton_clicked() {
	mTilesetScene->setExitVisible(mUi->showTilesetExitButton->isChecked());
}

// -----------------------------------------------------------------------------
bool MainWindow::tryCloseProject() {
	QVector<MapScene*> modifiedMaps;

	for (QGraphicsView *view : mViews) {
		MapScene *scene = dynamic_cast<MapScene*>(view->scene());
		if (scene->isModified()) {
			modifiedMaps.push_back(scene);
		}
	}

	if (modifiedMaps.isEmpty()) {
		closeProject();
		return true;
	}

	bool success = true;
	int result = QMessageBox::question(this, tr("Close project"), tr("At least one map has not been saved yet. Do you want to save all changes?"), QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
	switch (result) {
		case QMessageBox::Yes:
			for (MapScene *scene : modifiedMaps) {
				if (!scene->save()) {
					success = false;
				}
			}
			if (!success) {
				QMessageBox::critical(this, tr("Save map"), tr("At least one map could not be saved."));
			}
			break;

		case QMessageBox::No:
			break;

		case QMessageBox::Cancel:
			return false;
	}

	if (success) {
		closeProject();
	}

	return success;
}

// -----------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent *event) {
	if (tryCloseProject()) {
		QMainWindow::closeEvent(event);
	} else {
		event->ignore();
	}
}

// -----------------------------------------------------------------------------
void MainWindow::on_actionQuit_triggered() {
	if (tryCloseProject()) {
		qApp->quit();
	}
}

// -----------------------------------------------------------------------------
void MainWindow::on_actionTilesetEditor_triggered() {
	TilesetEditorDialog dialog(context().project(), this);
	dialog.exec();
	mContext->forceRedraw();
	redrawScenes();
}

// -----------------------------------------------------------------------------
void MainWindow::on_actionSpriteEditor_triggered() {
	SpriteEditorDialog dialog(context().project(), this);
	dialog.exec();
}

// -----------------------------------------------------------------------------
void MainWindow::onEditToolChanged() {
	mUi->actionBrushTool->setChecked(mContext->tool() == MapEditTool::BRUSH);
	mUi->actionFillTool->setChecked(mContext->tool() == MapEditTool::FILL);
	mUi->actionNpcEditorTool->setChecked(mContext->tool() == MapEditTool::OBJECT);
}

// -----------------------------------------------------------------------------
void MainWindow::on_actionNpcEditorTool_triggered() {
	mContext->setToolWithValidLayer(MapEditTool::OBJECT);
	onEditToolChanged();
}

// -----------------------------------------------------------------------------
void MainWindow::on_actionBrushTool_triggered() {
	mContext->setToolWithValidLayer(MapEditTool::BRUSH);
	onEditToolChanged();
}

// -----------------------------------------------------------------------------
void MainWindow::on_actionFillTool_triggered() {
	mContext->setToolWithValidLayer(MapEditTool::FILL);
	onEditToolChanged();
}

// -----------------------------------------------------------------------------
void MainWindow::on_actionToggleGrid_triggered() {
	mGridEnabled = mUi->actionToggleGrid->isChecked();

	for (QGraphicsView *view : mViews) {
		(static_cast<MapScene*>(view->scene()))->setGridEnabled(mGridEnabled);
	}
}

// -----------------------------------------------------------------------------
void MainWindow::on_projectTree_itemDoubleClicked(QTreeWidgetItem *item, int column) {
	(void)column;

	if (item->data(1, Qt::DisplayRole).toString() == "") {
		return;
	}

	openViewById(item->data(1, Qt::DisplayRole).toString());
}

// -----------------------------------------------------------------------------
void MainWindow::openViewById(const QString &id) {
	QGraphicsView *view = getView(id);
	mUi->tabMap->setCurrentWidget(view);
}

// -----------------------------------------------------------------------------
void MainWindow::on_tabMap_tabCloseRequested(int index) {
	if (!getTabScene(index)) {
		return;
	}

	if (getTabScene(index)->isModified()) {
		int result = QMessageBox::question(this, tr("Close map"), tr("The map has been modified. Do you want to save it?"), QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);

		switch (result) {
			case QMessageBox::Yes:
				if (!getTabScene(index)->save()) {
					QMessageBox::critical(this, tr("Save map"), tr("Could not save map due to errors"));
					return;
				}
				break;
			case QMessageBox::No:
				break;
			case QMessageBox::Cancel:
				return;
		}
	}

	removeView(getTabScene(index)->id());
}

// -----------------------------------------------------------------------------
void MainWindow::on_actionUndo_triggered() {
	if (!mCurrentUndoStack) {
		return;
	}

	mCurrentUndoStack->undo();
}

// -----------------------------------------------------------------------------
void MainWindow::on_actionRedo_triggered() {
	if (!mCurrentUndoStack) {
		return;
	}

	mCurrentUndoStack->redo();
}

// -----------------------------------------------------------------------------
void MainWindow::setCanUndoRedo() {
	if (!mUi) {
		return;
	}

	Aoba::log::todo("TEST if setCanUndoRedo is connected only once per scene!");

	if (!mCurrentUndoStack) {
		mUi->actionUndo->setEnabled(false);
		mUi->actionRedo->setEnabled(false);
		return;
	}

	mUi->actionUndo->setEnabled(mCurrentUndoStack->canUndo());
	mUi->actionRedo->setEnabled(mCurrentUndoStack->canRedo());
}

// -----------------------------------------------------------------------------
void MainWindow::on_tabMap_currentChanged(int index) {
	if (mCurrentUndoStack) {
		disconnect(mCurrentUndoStack);
		mCurrentUndoStack = nullptr;
	}

	if (index == -1) {
	  mContext->setTileset(nullptr);
		mContext->setLayerList(nullptr);
		mUi->currentNpc->setLevel(nullptr);
		updateUi();
		return;
	}
	
	MapScene *scene = currentScene();
	if (scene) {
		mUi->mapNameEdit->setText(scene->level()->asset()->name());
		mUi->mapWidthEdit->setValue(scene->tilemap()->asset()->width());
		mUi->mapHeightEdit->setValue(scene->tilemap()->asset()->height());

	  mContext->setTileset(scene->tileset()->asset());
		mContext->setLayerList(scene->layers());
		if (!scene->layers()->all().isEmpty()) {
			mContext->setEditLayer(scene->layers()->all().front());
		}
		scene->redrawTilemap();
		mCurrentUndoStack = scene->undoStack();
		connect(mCurrentUndoStack, &QUndoStack::canRedoChanged, this, &MainWindow::setCanUndoRedo);
		connect(mCurrentUndoStack, &QUndoStack::canUndoChanged, this, &MainWindow::setCanUndoRedo);
		connect(scene, &MapScene::modifiedChanged, this, &MainWindow::onMapModifiedChanged);
		connect(scene, &MapScene::selectionChanged, this, &MainWindow::onMapSceneSelectionChanged);
		updateUi();

		mUi->layers->selectedRowChanged();
		mUi->currentNpc->setLevel(scene->level()->asset());
		setSelectedTerrainLayer();
		setSelectedTilemapLayer();
	}
}

// -----------------------------------------------------------------------------
void MainWindow::updateTabName(int index) {
	MapScene *scene = getTabScene(index);
	if (!scene || !scene->valid()) {
		return;
	}

	QString name = scene->level()->asset()->name();
	if (scene->isModified()) {
		name += " (*)";
	}
	mUi->tabMap->setTabText(index, name);
}

// -----------------------------------------------------------------------------
void MainWindow::onMapModifiedChanged() {
	int numTabs = mUi->tabMap->count();
	for (int i=0; i<numTabs; i++) {
		updateTabName(i);
	}
}

// -----------------------------------------------------------------------------
void MainWindow::on_actionZoomIn_triggered() {
	context().localConfig().zoom().zoomIn();
	updateZoom();
}

// -----------------------------------------------------------------------------
void MainWindow::on_actionZoomOut_triggered() {
	context().localConfig().zoom().zoomOut();
	updateZoom();
}

// -----------------------------------------------------------------------------
void MainWindow::on_actionZoom100_triggered() {
	context().localConfig().zoom().set(2.0f);
	updateZoom();
}

// -----------------------------------------------------------------------------
void MainWindow::on_actionMapSave_triggered() {
	if (!currentScene()) {
		return;
	}

	if (!currentScene()->save()) {
		QMessageBox::critical(this, tr("Save map"), tr("Could not save map"));
	}
}

// -----------------------------------------------------------------------------
void MainWindow::updateZoom() {
	float zoom = context().localConfig().zoom().get();

	for (QGraphicsView *view : mViews) {
		view->resetTransform();
		view->scale(zoom, zoom);
	}
}

// -----------------------------------------------------------------------------
void MainWindow::updateUi() {
	if (!mUi) {
		return;
	}

	bool hasProject = !!context().project();
	bool hasScene = !!currentScene();

	mUi->actionProjectClose->setEnabled(hasProject);
	mUi->actionSpriteEditor->setEnabled(hasProject);
	mUi->actionTilesetEditor->setEnabled(hasProject);
	mUi->actionGameDiskCreate->setEnabled(hasProject);
	mUi->actionPlayTest->setEnabled(hasProject);
	
	mUi->newMapButton->setEnabled(hasProject);

	mUi->actionResizeMap->setEnabled(hasScene);
	mUi->mapNameEdit->setEnabled(hasScene);
	mUi->mapWidthEdit->setEnabled(hasScene);
	mUi->mapHeightEdit->setEnabled(hasScene);
	mUi->resizeMapButton->setEnabled(hasScene);
	mUi->actionMapSave->setEnabled(hasScene);
	mUi->actionMapRevert->setEnabled(hasScene);
	mUi->actionZoom100->setEnabled(hasScene);
	mUi->actionZoomIn->setEnabled(hasScene);
	mUi->actionZoomOut->setEnabled(hasScene);

	updateUiLayers();
	setCanUndoRedo();
}

// -----------------------------------------------------------------------------
void MainWindow::updateUiLayers() {
	if (!currentScene()) {
		return;
	}

	Maker::GameTilemapAsset *tilemap = currentScene()->tilemap();
	if (!tilemap) {
		return;
	}

	int index = 0;
	for (Aoba::TilemapAssetSetLayer *layer : tilemap->asset()->assetSet()->layers()) {
		if (mUi->tilesetLayerSelectBar->count() <= index) {
			mUi->tilesetLayerSelectBar->addTab(layer->name());
		} else {
			mUi->tilesetLayerSelectBar->setTabText(index, layer->name());
		}

		mUi->tilesetLayerSelectBar->setTabData(index++, layer->id());
	}

	while (mUi->tilesetLayerSelectBar->count() > index) {
		mUi->tilesetLayerSelectBar->removeTab(index);
	}

	on_tilesetLayerSelectBar_currentChanged(mUi->tilesetLayerSelectBar->currentIndex());
}

// -----------------------------------------------------------------------------
void MainWindow::setSelectedLayer() {
	MapSceneLayer *layer = mContext->editLayer();
	if (!layer) {
		return;
	}

	if (layer->isObjectLayer()) {
		MapSceneObjectLayer *objLayer = dynamic_cast<MapSceneObjectLayer*>(layer);
		
		if (objLayer->objectType() == Aoba::LevelNpcComponent::TypeId()) {
			setCurrentTool(mUi->npcTool);
		}
	} else {
		resetAllSelections();
	}
}

// -----------------------------------------------------------------------------
void MainWindow::setSelectedTilemapLayer() {
	Aoba::TilemapAssetSetLayer *layer = mContext->tilemapLayer();
	if (layer == nullptr) {
		return;
	}

	setCurrentTool(mUi->tilesetTool);

	if (mUi->tilesetLayerSelectBar->tabData(mUi->tilesetLayerSelectBar->currentIndex()).toString() != layer->id()) {
		for (int i=0; i<mUi->tilesetLayerSelectBar->count(); i++) {
			if (mUi->tilesetLayerSelectBar->tabData(i).toString() != layer->id()) {
				continue;
			}

			mUi->tilesetLayerSelectBar->setCurrentIndex(i);
			break;
		}
	}

	mContext->setTileBrush(mTilesetScene->brush());
}

// -----------------------------------------------------------------------------
void MainWindow::setSelectedTerrainLayer() {
	QString layerId = mContext->terrainLayer();
	if (layerId.isEmpty()) {
		mUi->terrainListWidget->setLayer(nullptr);
		return;
	}
	setCurrentTool(mUi->terrainTool);

	if (!currentScene() || !currentScene()->tileset()->asset()->terrain()) {
		mUi->terrainListWidget->setLayer(nullptr);
		return;
	}
	
	Aoba::TilesetTerrain *terrain = currentScene()->tileset()->asset()->terrain();
	mUi->terrainListWidget->setLayer(terrain->layer(layerId));
}

// -----------------------------------------------------------------------------
void MainWindow::on_tilesetLayerSelectBar_currentChanged(int index) {
	if (index >= mUi->tilesetLayerSelectBar->count() || !currentScene()) {
		return;
	}

	QString id = mUi->tilesetLayerSelectBar->tabData(index).toString();
	if (id.isEmpty()) {
		return;
	}

	Maker::GameTilemapAsset *tilemap = currentScene()->tilemap();
	if (!tilemap) {
		return;
	}

	Aoba::TilemapAssetSetLayer *layerConfig = tilemap->asset()->assetSet()->layer(id);
	if (!layerConfig) {
		return;
	}

	mUi->layers->selectTilemapLayer(layerConfig);
}

// -----------------------------------------------------------------------------
QGraphicsView *MainWindow::getView(const QString &id) {
	QGraphicsView *view = mViews[id];
	if (view) {
		return view;
	}

	float zoom = context().localConfig().zoom().get();

	view = new AobaGraphicsView(this);
	view->setTransformationAnchor(QGraphicsView::NoAnchor);
	view->scale(zoom, zoom);

	MapScene *scene = new MapScene(id, view, mContext, this);
	if (!scene->valid()) {
		delete scene;
		delete view;
		mViews.remove(id);
		return nullptr;
	}

	scene->setGridEnabled(mGridEnabled);
	view->setScene(scene);

	mViews.insert(id, view);

	connect(scene, &MapScene::hoverChanged, this, &MainWindow::onHoverInfo);
	connect(mContext, &MapEditorContext::toolChanged, scene, &MapScene::updateTool);

	mUi->tabMap->addTab(view, QIcon(":/icons/share/old_map.png"), id);
	onMapModifiedChanged();
	return view;
}

// -----------------------------------------------------------------------------
void MainWindow::removeView(const QString &id) {
	if (!mViews.contains(id)) {
		return;
	}

	QGraphicsView* view = mViews[id];
	MapScene* scene = static_cast<MapScene*>(view->scene());
	if (mContext->layerList() == scene->layers()) {
		mContext->setLayerList(nullptr);
	}

	mViews.remove(id);
	delete scene;
	delete view;
}

// -----------------------------------------------------------------------------
void MainWindow::on_mapNameEdit_textChanged() {
	MapScene *scene = currentScene();
	if (scene == nullptr) {
		return;
	}

	if (mUi->mapNameEdit->text() == scene->level()->asset()->name()) {
		return;
	}

	QString id = scene->level()->abstractType()->id() + "/" + scene->level()->asset()->id();
	QString newName = mUi->mapNameEdit->text();
	if (newName.isEmpty()) {
		newName = scene->level()->asset()->id();
	}

	scene->level()->asset()->setName(newName);
	mUi->projectTree->rename(id, newName);
	updateTabName(mUi->tabMap->currentIndex());
}

// -----------------------------------------------------------------------------
MapScene *MainWindow::getScene(const QString &id) {
	if (!mViews.contains(id)) {
		return nullptr;
	}

	return (static_cast<MapScene*>(mViews[id]->scene()));
}

// -----------------------------------------------------------------------------
QGraphicsView *MainWindow::getTabView(int index) {
	if (index == -1) {
		return nullptr;
	}

	return (static_cast<QGraphicsView*>(mUi->tabMap->widget(index)));
}

// -----------------------------------------------------------------------------
MapScene *MainWindow::getTabScene(int index) {
	if (!getTabView(index)) {
		return nullptr;
	}

	QGraphicsView* view = getTabView(index);
	return (static_cast<MapScene*>(view->scene()));
}

// -----------------------------------------------------------------------------
QGraphicsView *MainWindow::currentView() {
	return static_cast<QGraphicsView*>(mUi->tabMap->currentWidget());
}

// -----------------------------------------------------------------------------
MapScene *MainWindow::currentScene() {
	QGraphicsView* view = currentView();
	if (!view) {
		return nullptr;
	}
	return (static_cast<MapScene*>(view->scene()));
}

