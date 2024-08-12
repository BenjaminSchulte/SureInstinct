#include <QGraphicsSceneMouseEvent>
#include <aoba/tileset/TilesetTile.hpp>
#include <aoba/palette/SnesPalette.hpp>
#include <aoba/palette/Palette.hpp>
#include <aoba/palette/PaletteGroup.hpp>
#include <aoba/image/PaletteImageAccessor.hpp>
#include <maker/common/AobaGrid.hpp>
#include <maker/tileset/TilesetEditor.hpp>
#include <maker/tileset/TilesetEditorScene.hpp>
#include <maker/tileset/TileRenderHelper.hpp>

// -----------------------------------------------------------------------------
TilesetEditorScene::TilesetEditorScene(QObject *parent)
  : QGraphicsScene(parent)
  , mGrid(new AobaGrid(QSize(16, 16), QSize(10, 10)))
{
	setSceneRect(QRect(0,0,10,10));

  mBackgroundRect = addRect(QRect(0, 0, 10, 10), QPen(Qt::NoPen), TileRenderHelper::transparentBrush());
  mBackgroundRect->setVisible(false);

  mTiles = new QGraphicsPixmapItem();
  mTiles->setVisible(false);

  addItem(mTiles);
  addItem(mGrid);

  mSelectionRect = addRect(QRect(0, 0, 16, 16));
  QPen selectPen(QColor(255, 255, 0, 255));
  selectPen.setWidthF(1.5);
  mSelectionRect->setPen(selectPen);
  mSelectionRect->setVisible(false);
}

// -----------------------------------------------------------------------------
TilesetEditorScene::~TilesetEditorScene() {
  clear();
}

// -----------------------------------------------------------------------------
void TilesetEditorScene::clear() {
  for (QGraphicsItem *item : mItems) {
    delete item;
  }
  mItems.clear();
}
  
// -----------------------------------------------------------------------------
void TilesetEditorScene::clearRect(const QRect &tilesRect) {
  for (int y=tilesRect.top(); y<=tilesRect.bottom(); y++) {
    for (int x=tilesRect.left(); x<=tilesRect.right(); x++) {
      int hash = x + y * 1000;
      if (mItems.contains(hash)) {
        delete mItems[hash];
        mItems.remove(hash);
      }
    }
  }
}

// -----------------------------------------------------------------------------
void TilesetEditorScene::addItemTo(int x, int y, QGraphicsItem *item) {
  int hash = x + y * 1000;
  if (mItems.contains(hash)) {
    delete mItems[hash];
    mItems.remove(hash);
  }
  mItems.insert(hash, item);
  addItem(item);
}

// -----------------------------------------------------------------------------
void TilesetEditorScene::setCollisionVisible(bool visible) {
  if (visible == mCollisionVisible) {
    return; 
  }

  mCollisionVisible = visible;
  redrawLayer();
}

// -----------------------------------------------------------------------------
void TilesetEditorScene::setPriorityVisible(bool visible) {
  if (visible == mPriorityVisible) {
    return; 
  }

  mPriorityVisible = visible;
  redrawLayer();
}

// -----------------------------------------------------------------------------
void TilesetEditorScene::setMovementVisible(bool visible) {
  if (visible == mMovementVisible) {
    return; 
  }

  mMovementVisible = visible;
  redrawLayer();
}

// -----------------------------------------------------------------------------
void TilesetEditorScene::setEventVisible(bool visible) {
  if (visible == mEventVisible) {
    return; 
  }

  mEventVisible = visible;
  redrawLayer();
}

// -----------------------------------------------------------------------------
void TilesetEditorScene::setExitVisible(bool visible) {
  if (visible == mExitVisible) {
    return; 
  }

  mExitVisible = visible;
  redrawLayer();
}

// -----------------------------------------------------------------------------
void TilesetEditorScene::setGridVisible(bool visible) {
  mGrid->setVisible(visible);
}

// -----------------------------------------------------------------------------
void TilesetEditorScene::setToolGrid(int size) {
  mGrid->setSubGridEnabled(size == 8 && tileSize().width() > 8);
  mGridSize = size;
  
  mSelection = QRect();
  mSelectionRect->setVisible(false);  
}

// -----------------------------------------------------------------------------
void TilesetEditorScene::setBrushSize(const QSize &size) {
  if (mBrushSize == size) {
    return;
  }

  mBrushSize = size;
  mSelection = QRect();
  mSelectionRect->setVisible(false);  
}

// -----------------------------------------------------------------------------
void TilesetEditorScene::setLayer(TilesetEditorTilesetLayer *layer) {
  mLayer = layer;
  mGrid->setDistance(tileSize());
  redrawLayer();
}

// -----------------------------------------------------------------------------
QSize TilesetEditorScene::tileSize() const {
  if (!mLayer) {
    return QSize(16, 16);
  }

  return mLayer->layer()->tileset()->assetSet()->tileSize();
}

// -----------------------------------------------------------------------------
QSize TilesetEditorScene::bgTileSize() const {
  if (!mLayer) {
    return QSize(8, 8);
  }

  return mLayer->layer()->tileset()->assetSet()->bgTileSize();
}

// -----------------------------------------------------------------------------
void TilesetEditorScene::redrawLayer() {
  if (!mLayer) {
    mBackgroundRect->setVisible(false);
    mTiles->setVisible(false);
    mSelectionRect->setVisible(false);
    return;
  }

  QSize tileImageSize(tileSize());
  QRect tilesRect(QPoint(0, 0), mLayer->size());
  mPixmap = QPixmap(mLayer->size().width() * tileImageSize.width(), mLayer->size().height() * tileImageSize.height());
  mPixmap.fill(QColor(0, 0, 0, 0));

  redrawLayer(tilesRect);

  setSceneRect(QRect(QPoint(0, 0), mPixmap.size()));
  mTiles->setPixmap(mPixmap);
  mTiles->setVisible(true);

  mBackgroundRect->setRect(sceneRect());
  mBackgroundRect->setVisible(true);
  mGrid->setSize(sceneRect().size());
  mGrid->setVisible(true);
}

// -----------------------------------------------------------------------------
void TilesetEditorScene::redrawLayer(const QRect &tilesRect) {
  clearRect(tilesRect);

  QSize tileImageSize(tileSize());
  QSize bgTileImageSize(bgTileSize());
  QRect renderRect(
    QPoint(tilesRect.left() * tileImageSize.width(), tilesRect.top() * tileImageSize.height()),
    QSize(tilesRect.width() * tileImageSize.width(), tilesRect.height() * tileImageSize.height())
  );

  TileRenderHelper render(&mPixmap);
  QPainter &painter = render.painter();
  painter.setBackgroundMode(Qt::TransparentMode);
  painter.setCompositionMode(QPainter::CompositionMode_Clear);
  painter.fillRect(renderRect, Qt::transparent);
  painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
  
  if (!mLayer) {
    return;
  }

  Aoba::TilesetAssetSet *config = mLayer->layer()->tileset()->assetSet();
  Aoba::TilesetTiles *tiles = mLayer->layer()->tileset()->tiles(mLayer->layer()->config()->tiles());
  Aoba::SnesPalettePtr snesPalette = tiles->previewPalette();
  int numColors = tiles->config()->paletteGroupSet()->paletteSet()->numSnesColors();

  for (int y=tilesRect.top(); y<=tilesRect.bottom(); y++) {
    for (int x=tilesRect.left(); x<=tilesRect.right(); x++) {
      const Aoba::TilesetTile *tile = mLayer->tileAt(QPoint(x, y));
      if (tile == nullptr) {
        return;
      }

      int toX = x * tileImageSize.width();
      int toY = y * tileImageSize.height();

      for (int prio=0; prio<2; prio++) {
        for (uint8_t bg=0; bg<mLayer->layer()->config()->numBackgrounds(); bg++) {
          for (int by=0; by<config->numBgTilesY(); by++) {
            for (int bx=0; bx<config->numBgTilesX(); bx++) {
              const auto &part = tile->tileLayer(bg)->constPart(bx, by);
              
              if (part.priority() == prio) {
                const Aoba::TilesetBgTile &bgTile = tiles->bgTiles()[part.bgTileIndex()];
                Aoba::SnesPaletteView palette = snesPalette->subPalette(bgTile.palette, numColors, true);

                QImagePtr image((Aoba::PaletteImageAccessor(bgTile.data)).assignPalette(palette));
                painter.drawImage(QPoint(toX + bx * bgTileImageSize.width(), toY + by * bgTileImageSize.height()), *image.get());
              }

              if (mPriorityVisible && prio) {
                render.renderPriority(toX + bx * bgTileImageSize.width(), toY + by * bgTileImageSize.height(), part.priority());
              }
            }
          }
        }
      }

      Rpg::TilesetTileCollision self = mLayer->collisionFor(QPoint(x, y));

      if (mCollisionVisible) {
        Rpg::TilesetTileCollision none;
        render.renderCollision(toX, toY, self, none, none, none, none);
      }

      for (QGraphicsItem *item : TileRenderHelper::createTileIcons(QPoint(toX, toY), self, mMovementVisible, mEventVisible, mExitVisible, true)) {
        addItemTo(x, y, item);
      }
    }
  }
}

// -----------------------------------------------------------------------------
QRect TilesetEditorScene::eventToSelection(QGraphicsSceneMouseEvent *event) const {
  if (!mLayer) {
    return QRect();
  }

  QPoint start(
    event->scenePos().x() / mGridSize,
    event->scenePos().y() / mGridSize
  );

  QSize tileImageSize(tileSize());
  QSize sceneSize(
    mLayer->size().width() * tileImageSize.width() / mGridSize,
    mLayer->size().height() * tileImageSize.height() / mGridSize
  );

  return QRect(start, mBrushSize).intersected(QRect(QPoint(0, 0), sceneSize));
}

// -----------------------------------------------------------------------------
void TilesetEditorScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    mIsPainting = true;
    emit editStarted(eventToSelection(event));
    mSelection = QRect();
    mouseMoveEvent(event);
  }
}

// -----------------------------------------------------------------------------
void TilesetEditorScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  QRect selection = eventToSelection(event);
  if (mSelection == selection) {
    return;
  }

  mSelectionRect->setRect(QRect(
    selection.left() * mGridSize,
    selection.top() * mGridSize,
    selection.width() * mGridSize,
    selection.height() * mGridSize
  ));
  mSelectionRect->setVisible(true);
  mSelection = selection;

  if (mIsPainting) {
    emit editTriggered(selection);
  }
}

// -----------------------------------------------------------------------------
void TilesetEditorScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    mIsPainting = false;

    emit editFinished();
  }
}

// -----------------------------------------------------------------------------
bool TilesetEditorScene::event(QEvent *event) {
  if (event->type() == QEvent::Leave) {
    mSelectionRect->setVisible(false);
    mSelection = QRect();
  }

  return QGraphicsScene::event(event);
}