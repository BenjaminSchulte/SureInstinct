#pragma once

#include <QFile>
#include <QMainWindow>
#include <QVector>
#include <QSettings>

namespace Ui {
class MainWindow;
}

struct MapSceneHoverInfo;
class MapEditorContext;
class QTreeWidgetItem;
class QGraphicsView;
class QUndoStack;
class MapScene;
class TilesetScene;

struct TileBrush;

class MainWindow : public QMainWindow {
	Q_OBJECT
	
public:
  //! Constructor
	explicit MainWindow(QWidget *parent = nullptr);

  //! Deconstructor
	~MainWindow();

  //! Loads the last project
	void loadLastProject();

  //! Loads the last project
	void loadProject(const QString &folderName);

private slots:
	void onHoverInfo(const MapSceneHoverInfo &);

	void on_actionQuit_triggered();
	void on_actionSpriteEditor_triggered();
	void on_actionTilesetEditor_triggered();
	void on_actionResizeMap_triggered();
	void on_actionProjectClose_triggered();
	void on_newMapButton_clicked();

	void on_actionZoomIn_triggered();
	void on_actionZoomOut_triggered();
	void on_actionZoom100_triggered();

	void on_projectTree_itemDoubleClicked(QTreeWidgetItem *item, int column);

	void on_tabMap_tabCloseRequested(int index);
	
	void on_tabMap_currentChanged(int index);

	void on_tilesetLayerSelectBar_currentChanged(int index);

	void on_actionToggleGrid_triggered();

	void on_actionBrushTool_triggered();
	void on_actionFillTool_triggered();
	void on_actionNpcEditorTool_triggered();
	void on_actionMirrorX_triggered();
	void on_actionMirrorY_triggered();

	void on_actionMapSave_triggered();
	void on_actionUndo_triggered();
	void on_actionRedo_triggered();
	void setCanUndoRedo();

	void onEditToolChanged();
	void setSelectedLayer();
	void setSelectedTilemapLayer();
	void setSelectedTerrainLayer();
	void on_showTilesetGridButton_clicked();
	void on_showTilesetCollisionButton_clicked();
	void on_showTilesetEnterButton_clicked();
	void on_showTilesetMoveButton_clicked();
	void on_showTilesetExitButton_clicked();

	void on_mapNameEdit_textChanged();
	void onMapModifiedChanged();
	void onMapSceneSelectionChanged();
	void resetAllSelections();

	void redrawScenes();


protected:
	void closeEvent(QCloseEvent *) override;

private:
	//! Moves the tile selection of the current tool
	void moveTileSelection(int x, int y);

	//! Sets the current tool
	void setCurrentTool(QWidget *widget);

	//! Try to close the project
	bool tryCloseProject();

	//! Closes the project
	void closeProject();

	//! Updates the name of a tab
	void updateTabName(int);

	//! Updates the UI layers
	void updateUi();

	//! Updates the UI layers
	void updateUiLayers();

	//! Opens a view
	void openViewById(const QString &id);

	//! Returns the graphics view
	QGraphicsView *getView(const QString &id);
	
	//! Returns the scene
	MapScene *getScene(const QString &id);

	//! Returns the graphics view
	QGraphicsView *getTabView(int index);
	
	//! Returns the scene
	MapScene *getTabScene(int index);

	//! Returns the current scene
	MapScene *currentScene();

	//! Returns the current scene
	QGraphicsView *currentView();

	//! Closes a view
	void removeView(const QString &id);

	//! Updates the zoom
	void updateZoom();

	//! Whether the grid is enabled
	bool mGridEnabled = true;

  //! The UI
	Ui::MainWindow *mUi;

	//! Main context
	MapEditorContext *mContext;

	//! The palette scene
	TilesetScene *mTilesetScene;

	//! List of all tabs
	QMap<QString, QGraphicsView*> mViews;

  //! Settings
	QSettings mSettings;

	//! The current undo stack
	QUndoStack *mCurrentUndoStack = nullptr;
};


