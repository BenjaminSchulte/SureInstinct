#pragma once

#include <QMainWindow>
#include <QMap>
#include <aoba/tileset/Tileset.hpp>
#include <maker/tileset/TilemapReader.hpp>
#include <rpg/tileset/TilesetCollisionModule.hpp>

typedef QSharedPointer<class MakerProject> MakerProjectPtr;
typedef QSharedPointer<QImage> QImagePtr;

class QComboBox;
class QToolButton;

namespace Maker {
class GameTilesetAssetType;
}

namespace Ui {
class TilesetEditor;
}

class TilesetEditor;
class TilesetEditorScene;
class OriginalTilesetScene;

class TilesetEditorTilesetLayer {
public:
  //! Constructor
  TilesetEditorTilesetLayer(Aoba::TilesetLayer *layer);

  //! Deconstructor
  ~TilesetEditorTilesetLayer();

  //! Number of tiles per row
  int numTilesPerRow() const;

  //! Returns the size
  QSize size() const;

  //! Adds a tile row
  void addTileRow();

  //! Returns the tileset layer
  inline Aoba::TilesetLayer* layer() const { return mLayer; }

  //! Returns the tile at a position
  Aoba::TilesetTile *tileAt(const QPoint &);

  //! Returns the tile at a position
  Aoba::TilesetTile *requireTileAt(const QPoint &);

  //! Returns collision for
  Rpg::TilesetTileCollision collisionFor(const QPoint &);

  //! Returns collision for
  Rpg::TilesetTileCollision collisionFor(Aoba::TilesetTile *) const;

  //! Returns collision for
  Rpg::TilesetTileCollision &requestCollisionFor(const QPoint &);

  //! Returns all tiles
  inline const QVector<Aoba::TilesetTile*> tiles() const { return mTiles; }

private:
  //! The layer
  Aoba::TilesetLayer *mLayer;

  //! List of all tiles
  QVector<Aoba::TilesetTile*> mTiles;

  //! List of all collisions
  QMap<Aoba::TilesetTile*, Rpg::TilesetTileCollision> mCollisions;
};

class TilesetEditorTileset {
public:
  //! Constructor
  TilesetEditorTileset(Aoba::Tileset *tileset);

  //! Deconstructor
  ~TilesetEditorTileset();

  //! The tileset
  inline Aoba::Tileset *tileset() const { return mTileset; }

  //! Returns the layer
  TilesetEditorTilesetLayer *layer(Aoba::TilesetLayer *layer);

  //! Returns whether the tileset has been modified
  inline bool isModified() const { return mModified; }

  //! Sets the modified flag
  inline void setIsModified() { mModified = true; }

private:
  //! The tileset
  Aoba::Tileset *mTileset = nullptr;

  //! All layers
  QMap<Aoba::TilesetLayer*, TilesetEditorTilesetLayer*> mLayers;

  //! Whether the tileset has been modified
  bool mModified = false;
};

class TilesetEditorTool {
public:
  enum Tool {
    INVALID,
    PRIORITY_EDGE,
    COLLISION_EDGE,
    COLLISION_GROUP,
    ENTER_EVENT,
    MOVE_EFFECT,
    EXIT_FLAG,
    GRAPHIC
  };

  TilesetEditorTool(Tool tool=INVALID, int id=0)
    : mTool(tool)
    , mId(id)
  {}

  virtual ~TilesetEditorTool() = default;

  inline Tool tool() const { return mTool; }
  inline int id() const { return mId; }

  virtual void startEdit(TilesetEditorTilesetLayer*, const QRect &) {}
  virtual void triggerEdit(TilesetEditorTilesetLayer*, const QRect &) {}
  virtual void finishEdit(TilesetEditorTilesetLayer*) {}

  inline void setCurrentTileLayer(uint8_t tileLayer) { mTileLayer = tileLayer; }
  bool editBgTiles() const;
  bool requiresCollisionLayer() const;

protected:
  Tool mTool;
  int mId;
  uint8_t mTileLayer = 0;
};

class TilesetEditorPriorityTool : public TilesetEditorTool {
public:
  TilesetEditorPriorityTool() : TilesetEditorTool(PRIORITY_EDGE) {}

  void startEdit(TilesetEditorTilesetLayer*, const QRect &) override;
  void triggerEdit(TilesetEditorTilesetLayer*, const QRect &) override;

private:
  bool mOldValue;
};

class TilesetEditorCollisionEdgeTool : public TilesetEditorTool {
public:
  TilesetEditorCollisionEdgeTool() : TilesetEditorTool(COLLISION_EDGE) {}

  void startEdit(TilesetEditorTilesetLayer*, const QRect &) override;
  void triggerEdit(TilesetEditorTilesetLayer*, const QRect &) override;

private:
  bool mOldValue;
};

class TilesetEditorExitTool : public TilesetEditorTool {
public:
  TilesetEditorExitTool() : TilesetEditorTool(EXIT_FLAG) {}

  void startEdit(TilesetEditorTilesetLayer*, const QRect &) override;
  void triggerEdit(TilesetEditorTilesetLayer*, const QRect &) override;

private:
  bool mOldValue;
};

class TilesetEditorCollisionGroupTool : public TilesetEditorTool {
public:
  TilesetEditorCollisionGroupTool(int id) : TilesetEditorTool(COLLISION_GROUP, id) {}
  void triggerEdit(TilesetEditorTilesetLayer*, const QRect &) override;
};

class TilesetEditorEnterEventTool : public TilesetEditorTool {
public:
  TilesetEditorEnterEventTool(int id) : TilesetEditorTool(ENTER_EVENT, id) {}
  void triggerEdit(TilesetEditorTilesetLayer*, const QRect &) override;
};

class TilesetEditorMoveEffectTool : public TilesetEditorTool {
public:
  TilesetEditorMoveEffectTool(int id) : TilesetEditorTool(MOVE_EFFECT, id) {}
  void triggerEdit(TilesetEditorTilesetLayer*, const QRect &) override;
};

class TilesetEditorGraphicTool : public TilesetEditorTool {
public:
  TilesetEditorGraphicTool(TilesetEditor *editor) : TilesetEditorTool(GRAPHIC), mEditor(editor) {}
  void triggerEdit(TilesetEditorTilesetLayer*, const QRect &) override;
private:  
  TilesetEditor *mEditor;
};

class TilesetEditor : public QMainWindow {
  Q_OBJECT

public:
  //! Constructor
  TilesetEditor(const MakerProjectPtr &project, QWidget *parent);

  //! Deconstructor
  ~TilesetEditor();

  //! Saves the changed
  bool save() const;

  //! Returns the brush
  QRect selectedTiles() const;

public slots:
  void startEdit(const QRect &);
  void triggerEdit(const QRect &);
  void finishEdit();

private slots:
  void onTilesetListCurrentIndexChanged(int index);
  void onLayerListCurrentIndexChanged(int index);

  void on_toolPriority_clicked();
  void on_toolCollision_clicked();
  void on_toolCollisionType0_clicked();
  void on_toolCollisionType1_clicked();
  void on_toolCollisionType2_clicked();
  void on_toolCollisionType3_clicked();
  void on_toolExit_clicked();
  void on_toolEnter0_clicked();
  void on_toolEnter2_clicked();
  void on_toolEnter3_clicked();
  void on_toolEnter4_clicked();
  void on_toolEnter5_clicked();
  void on_toolEnter6_clicked();
  void on_toolEnter7_clicked();
  void on_toolMove0_clicked();
  void on_toolMove1_clicked();
  void on_toolMove2_clicked();
  void on_toolMove3_clicked();
  void on_toolMove4_clicked();
  void on_toolMove6_clicked();
  void on_toolMove8_clicked();
  void on_toolMove9_clicked();
  void on_toolMove10_clicked();
  void on_toolMove11_clicked();
  void onGraphicsClicked();

  void on_actionGrid_triggered();
  void on_actionAddTiles_triggered();

  void setCurrentLayerBg(int);

private:
  //! Saves a layer
  bool save(TilesetEditorTileset*) const;

  //! Sets the current tool
  void setTool(TilesetEditorTool *tool);

  //! Whether collision is available
  bool hasCollisionLayer() const;

  //! Tests whether the tool is still valid
  void testToolIsValid();

  //! Updates the UI
  void updateUi();

  //! Updates the UI tool
  void updateUiTool();

  //! Updates the tileset list
  void updateTilesetList();

  //! Updates the layer list
  void updateLayerList();

  //! The UI
  Ui::TilesetEditor *mUi;

  //! The project
  MakerProjectPtr mProject;
  
  //! The current tool
  TilesetEditorTool *mTool = nullptr;

  //! The current collision ID
  uint8_t mCollisionType = 0;

  //! Tileset list
  QVector<Maker::GameTilesetAssetType*> mTilesets;

  //! The tileset list
  QComboBox *mTilesetList;

  //! The layer list
  QComboBox *mLayerList;

  //! The current tileset
  TilesetEditorTileset *mTileset = nullptr;

  //! The current layer
  TilesetEditorTilesetLayer *mLayer = nullptr;

  //! List of all loaded tilesets
  QMap<QString, TilesetEditorTileset*> mLoadedTilesets;

  //! All enter event buttons
  QVector<QToolButton*> mEnterEvents;

  //! All move effect buttons
  QVector<QToolButton*> mMoveEffects;

  //! The editor scene
  TilesetEditorScene *mEditorScene;

  //! The editor scene
  OriginalTilesetScene *mTilesScene;

  //! The tab select for the tileset layer
  QTabBar *mTileLayerSelect;
};