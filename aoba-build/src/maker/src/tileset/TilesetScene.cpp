#include <aoba/log/Log.hpp>
#include <QPixmap>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/tilemap/Tilemap.hpp>
#include <rpg/tileset/TilesetCollisionModule.hpp>
#include <maker/game/GameTilesetAssetType.hpp>
#include <maker/common/AobaGraphicsView.hpp>
#include <maker/common/AobaGrid.hpp>
#include <maker/tileset/TilesetScene.hpp>
#include <maker/tileset/TilemapReader.hpp>
#include <maker/tileset/TileBrush.hpp>
#include <maker/tileset/TileRenderHelper.hpp>

// -----------------------------------------------------------------------------
TilesetScene::TilesetScene(TilemapReader *context, QObject *parent)
  : QGraphicsScene(parent)
  , mReader(context)
  , mSelection(QSize(1, 1), QSize(16, 16))
  , mGrid(new AobaGrid(QSize(16, 16), QSize(10, 10)))
{
	setSceneRect(QRect(0,0,10,10));

  connect(context, &TilemapReader::layersChanged, this, &TilesetScene::onTilesetChange);
  connect(context, &TilemapReader::sizeChanged, this, &TilesetScene::onLayerChange);
  connect(context, &TilemapReader::tilesChanged, this, &TilesetScene::onLayerChange);

  mBackgroundRect = addRect(QRect(0, 0, 10, 10), QPen(Qt::NoPen), TileRenderHelper::transparentBrush());
  mBackgroundRect->setVisible(false);

  mTiles = new QGraphicsPixmapItem();
  mTiles->setVisible(false);
  addItem(mTiles);

  mGrid->setZValue(1);
  addItem(mGrid);
  
  mSelectionRect = addRect(QRect(0, 0, 16, 16));
  QPen selectPen(QColor(255, 255, 0, 255));
  selectPen.setWidthF(1.5);
  mSelectionRect->setPen(selectPen);
  mSelectionRect->setZValue(2);
}

// -----------------------------------------------------------------------------
TilesetScene::~TilesetScene() {
  clear();
  delete mGrid;
}

// -----------------------------------------------------------------------------
void TilesetScene::clear() {
  for (QGraphicsItem *item : mItems) {
    delete item;
  }
  mItems.clear();
  mLayers.clear();
}

// -----------------------------------------------------------------------------
void TilesetScene::setGridVisible(bool visible) {
  mGrid->setVisible(visible);
}

// -----------------------------------------------------------------------------
void TilesetScene::setCollisionVisible(bool visible) {
  mShowCollision = visible;
  onTilesetChange();
}

// -----------------------------------------------------------------------------
void TilesetScene::setExitVisible(bool visible) {
  mShowExit = visible;
  onTilesetChange();
}

// -----------------------------------------------------------------------------
void TilesetScene::setEnterVisible(bool visible) {
  mShowEnter = visible;
  onTilesetChange();
}

// -----------------------------------------------------------------------------
void TilesetScene::setMovementVisible(bool visible) {
  mShowMovement = visible;
  onTilesetChange();
}

// -----------------------------------------------------------------------------
void TilesetScene::selectTile(int x, int y) {
  mSelection.move(x, y);
  updateSelectionRect();
  setSelectionFromRect();
}

// -----------------------------------------------------------------------------
QSize TilesetScene::tileSize() const {
  if (!mTileset) {
    return QSize(16, 16);
  }

  return mTileset->assetSet()->tileSize();
}

// -----------------------------------------------------------------------------
QSize TilesetScene::bgTileSize() const {
  if (!mTileset) {
    return QSize(8, 8);
  }

  return mTileset->assetSet()->bgTileSize();
}

// -----------------------------------------------------------------------------
void TilesetScene::onLayerChange() {
  if (mTileset == nullptr || mReader->layers().isEmpty()) {
    mLayer = nullptr;
    mTiles->setVisible(false);
    mBackgroundRect->setVisible(false);
    mGrid->setSize(QSize(0, 0));
  	setSceneRect(QRect(0,0,10,10));
    mSelection.setMapSize(QSize(0, 0));
    updateSelectionRect();
    setSelectionFromRect();
    return;
  }

  mLayer = mReader->layers().first();
  QRect newSelection(0, 0, 1, 1);
  if (mLayerSelection.contains(mLayer)) {
    newSelection = mLayerSelection[mLayer];
  }

  QSize tileImageSize(tileSize());

  const QPixmap &pixmap = mLayers[mLayer].pixmap;
  mTiles->setPixmap(pixmap);
  mTiles->setVisible(true);

  setSceneRect(QRect(0, 0, pixmap.width(), pixmap.height()));
  mGrid->setSize(sceneRect().size());
  mGrid->setDistance(tileImageSize);
  mSelection.setTileSize(tileImageSize);
  mSelection.setMapSize(QSize(pixmap.width() / tileImageSize.width(), pixmap.height() / tileImageSize.height()));
  mSelection.setSelection(newSelection);
  mBackgroundRect->setRect(sceneRect());
  mBackgroundRect->setVisible(true);
  updateSelectionRect();
  setSelectionFromRect();
}

// -----------------------------------------------------------------------------
void TilesetScene::onTilesetChange() {
  if (mReader->layers().isEmpty()) {
    mTileset = nullptr;
    onLayerChange();
    return;
  }

  mTiles->setVisible(true);

  mTileset = mReader->layers().first()->tileset();
  int tileWidth = mTileset->assetSet()->tileWidth();
  int tileHeight = mTileset->assetSet()->tileHeight();

  clear();
  for (Aoba::TilesetAssetSetLayer *layerConfig : mTileset->assetSet()->layer()) {
    Aoba::TilesetLayer *layer = mTileset->layer(layerConfig);

    TilesetSceneLayer &ourLayer = mLayers[layer];

    int numTilesX = mReader->size().width();
    int numTilesY = mReader->size().height();
    
    ourLayer.pixmap = QPixmap(numTilesX * tileWidth, numTilesY * tileHeight);
    ourLayer.pixmap.fill(QColor(0, 0, 0, 0));

    renderTiles(ourLayer, layer, QRect(0, 0, numTilesX, numTilesY));

    for (Aoba::TilesetAssetSetModule *mod : mTileset->assetSet()->modules()) {
      Aoba::TilesetAssetSetModuleInstance *instance = mod->instanceFor(mTileset);
      
      if (instance->typeId() == Rpg::TilesetCollisionModuleInstance::TypeId()) {
        Rpg::TilesetCollisionModuleInstance *col = dynamic_cast<Rpg::TilesetCollisionModuleInstance*>(instance);
        renderCollision(layer, col);
      }
    }
  }

  onLayerChange();
}

// -----------------------------------------------------------------------------
void TilesetScene::renderCollision(Aoba::TilesetLayer *layer, Rpg::TilesetCollisionModuleInstance *col) {
  int numTilesX = mReader->size().width();
  int numTilesY = mReader->size().height();
  int tileWidth = mTileset->assetSet()->tileWidth();
  int tileHeight = mTileset->assetSet()->tileHeight();

  TilesetSceneLayer &ourLayer = mLayers[layer];
  TileRenderHelper render(&ourLayer.pixmap);
  
  for (int y=0; y<numTilesY; y++) {
    for (int x=0; x<numTilesX; x++) {
      int index = mReader->tileAt(layer, QPoint(x, y)).index;
      Rpg::TilesetTileCollision self = col->forTile(layer->allTiles()[index]);
      Rpg::TilesetTileCollision none;
      if (mShowCollision) {
        render.renderCollision(x*tileWidth, y*tileHeight, self, none, none, none, none);
      }

      for (QGraphicsItem *item : TileRenderHelper::createTileIcons(QPoint(x * 16, y * 16), self, mShowMovement, mShowEnter, false, false)) {
        addItem(item);
        mItems.push_back(item);
      }

      if (mShowExit && self.exit) {
        QGraphicsItem *item = TileRenderHelper::createTileRect(QPoint(x * 16, y * 16), QPen(QColor(255, 0, 0, 255), 1.5), Qt::NoBrush);
        addItem(item);
        mItems.push_back(item);
      }
    }
  }
}

// -----------------------------------------------------------------------------
void TilesetScene::renderTiles(TilesetSceneLayer &ourLayer, Aoba::TilesetLayer *layer, const QRect &rect) {
  TileRenderHelper render(&ourLayer.pixmap);
  auto *tileset = mReader->layers().first()->tileset();
  int tileWidth = tileset->assetSet()->tileWidth();
  int tileHeight = tileset->assetSet()->tileHeight();

  for (int y=rect.top(); y<=rect.bottom(); y++) {
    for (int x=rect.left(); x<=rect.right(); x++) {
      Aoba::TilesetTileRef ref(mReader->tileAt(layer, QPoint(x, y)));
      render.renderTile(layer, ref.index, x * tileWidth, y * tileHeight, true, true, ref.flipX, ref.flipY);
    }
  }
}

// -----------------------------------------------------------------------------
void TilesetScene::updateSelectionRect() {
  if (mReader->layers().isEmpty()) {
    mSelectionRect->setVisible(false);
    return;
  }

  QSize tileImageSize(tileSize());
  mSelectionRect->setVisible(true);
  mSelectionRect->setRect(QRect(
    mSelection.selection().left() * tileImageSize.width(),
    mSelection.selection().top() * tileImageSize.height(),
    mSelection.selection().width() * tileImageSize.width(),
    mSelection.selection().height() * tileImageSize.height()
  ));
}

// -----------------------------------------------------------------------------
void TilesetScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  if (event->button() == Qt::RightButton) {
    mSelection.cancel();
    updateSelectionRect();
  } else if (event->button() == Qt::LeftButton) {
    mSelection.startSelection(event->scenePos());
    updateSelectionRect();
    QGraphicsScene::mousePressEvent(event);
  } else {
    QGraphicsScene::mousePressEvent(event);
  }
}

// -----------------------------------------------------------------------------
void TilesetScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  if (event->buttons() & Qt::LeftButton) {
    mSelection.moveSelection(event->scenePos());
    updateSelectionRect();
  }
}

// -----------------------------------------------------------------------------
void TilesetScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  mSelection.stopSelection(event->scenePos());
  updateSelectionRect();
	QGraphicsScene::mouseReleaseEvent(event);

  setSelectionFromRect();
}

// -----------------------------------------------------------------------------
void TilesetScene::setSelectionFromRect() {
  if (mReader->layers().isEmpty()) {
    emit selectionChanged(mTileBrush = TileBrush());
    return;
  }

  const QRect &rect = mSelection.confirmedSelection();
  mLayerSelection[mReader->layers().front()] = rect;

  QVector<TileBrushTile> tiles;
  for (int y=rect.top(); y<=rect.bottom(); y++) {
    for (int x=rect.left(); x<=rect.right(); x++) {
      tiles.push_back(TileBrushTile(x + y * mReader->size().width()));
    }
  }

  emit selectionChanged(mTileBrush = TileBrush(tiles, rect.size()));
}
