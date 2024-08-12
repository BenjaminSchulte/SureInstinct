#include <aoba/log/Log.hpp>
#include <QMessageBox>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QUndoStack>
#include <aoba/level/LevelTilemapComponent.hpp>
#include <maker/GlobalContext.hpp>
#include <maker/common/AobaGrid.hpp>
#include <maker/game/GameLevelAssetType.hpp>
#include <maker/game/GameTilemapAssetType.hpp>
#include <maker/game/GameTilesetAssetType.hpp>
#include <maker/project/MakerProject.hpp>
#include <maker/map/MapEditorContext.hpp>
#include <maker/tileset/TileRenderHelper.hpp>
#include <maker/map/MapScene.hpp>
#include <maker/map/MapSceneLayer.hpp>
#include <maker/map/MapSceneLayerList.hpp>
#include <maker/map/BrushMapSceneTool.hpp>
#include <maker/map/FillMapSceneTool.hpp>
#include <maker/map/ObjectMapSceneTool.hpp>

// -----------------------------------------------------------------------------
MapScene::MapScene(const QString &id, QGraphicsView *view, MapEditorContext *context,QObject *parent)
  : BaseTileScene(parent)
  , mLayers(new MapSceneLayerList(this))
  , mId(id)
  , mView(view)
  , mContext(context)
  , mUndoStack(new QUndoStack(this))
  , mGrid(new AobaGrid(QSize(16, 16), QSizeF(10, 10)))
{
  mView->setMouseTracking(true);
  mView->setInteractive(true);
  
  mBackgroundRect = addRect(QRect(0, 0, 10, 10), QPen(Qt::NoPen), TileRenderHelper::transparentBrush());
  mBackgroundRect->setVisible(false);

  mTilemapItem = addPixmap(mTilemapPixmap);
  mTilemapItem->setVisible(false);

  addItem(mGrid);

  connect(mUndoStack, &QUndoStack::indexChanged, this, &MapScene::onChanged);
  connect(mLayers, &MapSceneLayerList::visiblityChanged, this, QOverload<>::of(&MapScene::redrawTilemap));
  
  load();
}

// -----------------------------------------------------------------------------
MapScene::~MapScene() {
  delete mTool;
  delete mGrid;
  for (auto *layer : mLayers->all()) {
    if (layer == mContext->editLayer()) {
      mContext->setEditLayer(nullptr);
    }
  }
  mLayers->clear();
}

// -----------------------------------------------------------------------------
const TileBrush &MapScene::brush() const {
  return mContext->editLayer()->brush(this);
}

// -----------------------------------------------------------------------------
TileBrushTile MapScene::getTile(const QPoint &pos) const {
  return mContext->editLayer()->getTile(this, pos);
}

// -----------------------------------------------------------------------------
void MapScene::setTiles(const PositionedTileList &changes) {
  mContext->editLayer()->setTiles(this, changes);
  onChanged();
}

// -----------------------------------------------------------------------------
QSize MapScene::tileSize() const {
  if (!mTileset) {
    return QSize(16, 16);
  }

  return mTileset->asset()->assetSet()->tileSize();
}

// -----------------------------------------------------------------------------
QSize MapScene::bgTileSize() const {
  if (!mTileset) {
    return QSize(8, 8);
  }

  return mTileset->asset()->assetSet()->bgTileSize();
}

// -----------------------------------------------------------------------------
QSize MapScene::layerSize() const {
  if (!mTilemap) {
    return QSize();
  }

  return QSize(mTilemap->asset()->width(), mTilemap->asset()->height());
}

// -----------------------------------------------------------------------------
void MapScene::commitChanges() {
  mContext->editLayer()->commitSetTile(this);
  onChanged();
}

// -----------------------------------------------------------------------------
void MapScene::onChanged() {
  bool oldChanged = mChanged;

  mChanged = true;
  emit mapChanged();

  if (mChanged != oldChanged) {
    emit modifiedChanged(mChanged);
  }
}

// -----------------------------------------------------------------------------
void MapScene::unload() {
  mLevel = nullptr;
  mTilemap = nullptr;
  mTileset = nullptr;

  redrawTilemap();
}

// -----------------------------------------------------------------------------
bool MapScene::load() {
  unload();

  MakerProjectPtr project = GlobalContext::get()->project();
  if (!project) {
    return false;
  }

  //QStringList parts = mId.split('/');
  mLevel = project->getLevel(mId);
  if (!mLevel) {
    Aoba::log::warn("Unable to getLevel(): " + mId);
    unload();
    return false;
  }

  mTilemap = mLevel->tilemap();
  if (!mTilemap) {
    Aoba::log::warn("Unable to get level tilemap: " + mId);
    unload();
    return false;
  }

  mTileset = mTilemap->tileset();
  if (!mTileset) {
    Aoba::log::warn("Unable to get level tileset: " + mId);
    unload();
    return false;
  }

  mLayers->createLayersFromMap(this);

  mGrid->setDistance(tileSize());

  redrawTilemap();
  updateTool();
  
  return true;
}

// -----------------------------------------------------------------------------
bool MapScene::save() {
  bool successLevel = mLevel->asset()->save();
  bool successTilemap = mTilemap->asset()->save();

  if (!successLevel || !successTilemap) {
    return false;
  }

  mChanged = false;
  emit modifiedChanged(false);

  return true;
}

// -----------------------------------------------------------------------------
void MapScene::updateTool() {
  delete mTool;
  mTool = nullptr;

  switch (mContext->tool()) {
    case MapEditTool::BRUSH: mTool = new BrushMapSceneTool(); break;
    case MapEditTool::FILL: mTool = new FillMapSceneTool(); break;
    case MapEditTool::OBJECT: mTool = new ObjectMapSceneTool(); break;
  }

  mTool->initialize(this);
}

// -----------------------------------------------------------------------------
void MapScene::setGridEnabled(bool enabled) {
  mGrid->setVisible(enabled);
}

// -----------------------------------------------------------------------------
void MapScene::redrawTilemap() {
  if (!mTilemap || !mTileset) {
    mBackgroundRect->setVisible(false);
    mTilemapItem->setVisible(false);
    return;
  }

  QSize tileImageSize(tileSize());
  Aoba::Tilemap *tilemap = mTilemap->asset();
  mTilemapPixmap = QPixmap(tilemap->width() * tileImageSize.width(), tilemap->height() * tileImageSize.height());
  mTilemapPixmap.fill(QColor(0, 0, 0, 0));

  redrawTilemap(QRect(0, 0, tilemap->width(), tilemap->height()));

  mTilemapItem->setVisible(true);
  setSceneRect(QRect(0, 0, tilemap->width() * tileImageSize.width(), tilemap->height() * tileImageSize.height()));

  mBackgroundRect->setVisible(true);
  mBackgroundRect->setRect(sceneRect());
  mGrid->setSize(sceneRect().size());
}

// -----------------------------------------------------------------------------
void MapScene::redrawTilemap(const QRect &rect) {
  if (!mTilemap || !mTileset) {
    return;
  }

  MapSceneLayerRenderInfo render(this, rect.intersected(QRect(0, 0, mTilemap->asset()->width(), mTilemap->asset()->height())));
  QPainter painter(&mTilemapPixmap);
  painter.setBackgroundMode(Qt::TransparentMode);
  painter.setCompositionMode (QPainter::CompositionMode_Source);
  painter.fillRect(render.renderRect(), Qt::transparent);
  painter.setCompositionMode (QPainter::CompositionMode_SourceOver);

  for (MapSceneLayer *layer : mLayers->all()) {
    if (!layer->isVisible()) {
      continue;
    }

    layer->renderScreen(render);
  }

  painter.drawPixmap(render.renderRect().topLeft(), render.subPixmap());
  painter.setOpacity(0.5);
  painter.drawPixmap(render.renderRect().topLeft(), render.mainPixmap());
  painter.setOpacity(1.0);
  painter.drawPixmap(render.renderRect().topLeft(), render.overlayPixmap());

  for (MapSceneLayer *layer : mLayers->all()) {
    if (!layer->isVisible()) {
      continue;
    }

    layer->renderOverlay(render, painter);
  }

  mTilemapItem->setPixmap(mTilemapPixmap);
}

// -----------------------------------------------------------------------------
void MapScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  if (mContext->editLayer() && mContext->editLayer()->isObjectLayer()) {
    return QGraphicsScene::mousePressEvent(event);
  }

  QSize tileImageSize(tileSize());
  QPoint position(
    event->scenePos().x() / tileImageSize.width(),
    event->scenePos().y() / tileImageSize.height()
  );

  bool validPosition = sceneRect().contains(event->scenePos());

  if (validPosition && mTool) {
    mTool->onMouseDown(position, event);
  }
}

// -----------------------------------------------------------------------------
void MapScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  if (mContext->editLayer() && mContext->editLayer()->isObjectLayer()) {
    return QGraphicsScene::mouseReleaseEvent(event);
  }

  QSize tileImageSize(tileSize());
  QPoint position(
    event->scenePos().x() / tileImageSize.width(),
    event->scenePos().y() / tileImageSize.height()
  );

  bool validPosition = sceneRect().contains(event->scenePos());

  if (validPosition && mTool) {
    mTool->onMouseUp(position, event);
  }

}

// -----------------------------------------------------------------------------
void MapScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  if (mContext->editLayer() && mContext->editLayer()->isObjectLayer()) {
    return QGraphicsScene::mouseMoveEvent(event);
  }

  QSize tileImageSize(tileSize());
  QPoint position(
    event->scenePos().x() / tileImageSize.width(),
    event->scenePos().y() / tileImageSize.height()
  );
  if (mCurrentTile == position && mMouseOnScene) {
    return;
  }
  mCurrentTile = position;
  mMouseOnScene = true;

  bool validPosition = sceneRect().contains(event->scenePos());

  if (validPosition) {
    emit hoverChanged(MapSceneHoverInfo(mCurrentTile));

    if (mTool) {
      mTool->onMouseMove(mCurrentTile, event);
    }
  } else {
    emit hoverChanged(MapSceneHoverInfo());

    if (mTool) {
      mTool->onMouseMoveOuter(mCurrentTile, event);
    }
  }
}

// -----------------------------------------------------------------------------
bool MapScene::event(QEvent *event) {
  if (event->type() == QEvent::Leave) {
    mMouseOnScene = false;
    if (mTool) {
      return mTool->onMouseLeave();
    }
  }

  return QGraphicsScene::event(event);
}