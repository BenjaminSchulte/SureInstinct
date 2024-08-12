#include <QGraphicsSceneMouseEvent>
#include <aoba/tileset/TilesetTile.hpp>
#include <aoba/palette/SnesPalette.hpp>
#include <aoba/palette/Palette.hpp>
#include <aoba/palette/PaletteGroup.hpp>
#include <aoba/image/PaletteImageAccessor.hpp>
#include <maker/common/AobaGrid.hpp>
#include <maker/tileset/TilesetEditor.hpp>
#include <maker/tileset/OriginalTilesetScene.hpp>
#include <maker/tileset/TileRenderHelper.hpp>

// -----------------------------------------------------------------------------
OriginalTilesetScene::OriginalTilesetScene(QObject *parent)
  : QGraphicsScene(parent)
  , mGrid(new AobaGrid(QSize(16, 16), QSize(10, 10)))
  , mSelectionHelper(QSize(0, 0), QSize(16, 16))
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
void OriginalTilesetScene::setLayer(Aoba::TilesetTiles *layer) {
  mLayer = layer;
  mSelection = QRect();
  mSelectionRect->setVisible(false);
  mGrid->setDistance(tileSize());
  mSelectionHelper.setTileSize(tileSize());
  emit selectionChanged(mSelection);
  redrawLayer();
}

// -----------------------------------------------------------------------------
QSize OriginalTilesetScene::tileSize() const {
  if (!mLayer) {
    return QSize(16, 16);
  }

  return mLayer->tileset()->assetSet()->tileSize();
}

// -----------------------------------------------------------------------------
QSize OriginalTilesetScene::bgTileSize() const {
  if (!mLayer) {
    return QSize(8, 8);
  }

  return mLayer->tileset()->assetSet()->bgTileSize();
}

// -----------------------------------------------------------------------------
void OriginalTilesetScene::setSelection(const QRect &selection) {
  if (selection == mSelection) {
    return;
  }

  mSelection = selection;
  QSize tileImageSize(tileSize());

  if (selection.size().isValid()) {
    mSelectionRect->setVisible(true);
    mSelectionRect->setRect(QRect(QPoint(
      selection.left() * tileImageSize.width(),
      selection.top() * tileImageSize.height()
    ), QSize(
      selection.width() * tileImageSize.width(),
      selection.height() * tileImageSize.height()
    )));
  } else {
    mSelectionRect->setVisible(false);
  }

  emit selectionChanged(selection);
}

// -----------------------------------------------------------------------------
void OriginalTilesetScene::redrawLayer() {
  if (!mLayer) {
    mBackgroundRect->setVisible(false);
    mTiles->setVisible(false);
    mSelectionRect->setVisible(false);
    return;
  }

  QSize tileImageSize(tileSize());
  QSize layerSize(
    mLayer->numTilesX(),
    mLayer->numTilesY()
  );
  QRect tilesRect(QPoint(0, 0), layerSize);
  mPixmap = QPixmap(layerSize.width() * tileImageSize.width(), layerSize.height() * tileImageSize.height());
  mPixmap.fill(QColor(0, 0, 0, 0));

  redrawLayer(tilesRect);

  setSceneRect(QRect(QPoint(0, 0), mPixmap.size()));
  mSelectionHelper.setMapSize(layerSize);
  mTiles->setPixmap(mPixmap);
  mTiles->setVisible(true);

  mBackgroundRect->setRect(sceneRect());
  mBackgroundRect->setVisible(true);
  mGrid->setSize(sceneRect().size());
  mGrid->setVisible(true);
}

// -----------------------------------------------------------------------------
void OriginalTilesetScene::redrawLayer(const QRect &tilesRect) {
  QSize tileImageSize(tileSize()); 
  QSize bgTileImageSize(bgTileSize()); 

  QRect renderRect(
    QPoint(tilesRect.left() * tileImageSize.width(), tilesRect.top() * tileImageSize.height()),
    QSize(tilesRect.width() * tileImageSize.width(), tilesRect.height() * tileImageSize.height())
  );

  QPainter painter(&mPixmap);
  painter.setBackgroundMode(Qt::TransparentMode);
  painter.setCompositionMode(QPainter::CompositionMode_Clear);
  painter.fillRect(renderRect, Qt::transparent);
  painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
  
  if (!mLayer) {
    return;
  }

  Aoba::TilesetAssetSet *config = mLayer->tileset()->assetSet();
  Aoba::SnesPalettePtr snesPalette = mLayer->previewPalette();
  int numColors = mLayer->config()->paletteGroupSet()->paletteSet()->numSnesColors();

  for (int y=tilesRect.top(); y<=tilesRect.bottom(); y++) {
    for (int x=tilesRect.left(); x<=tilesRect.right(); x++) {
      for (int by=0; by<config->numBgTilesY(); by++) {
        for (int bx=0; bx<config->numBgTilesX(); bx++) {
          int index = (x * config->numBgTilesX() + bx) + (y * config->numBgTilesY() + by) * mLayer->numBgTilesX();
          if (index >= mLayer->bgTiles().count()) {
            continue;
          }

          const Aoba::TilesetBgTile &bgTile = mLayer->bgTiles()[index];
          Aoba::SnesPaletteView palette = snesPalette->subPalette(bgTile.palette, numColors, true);

          QImagePtr image((Aoba::PaletteImageAccessor(bgTile.data)).assignPalette(palette));
          painter.drawImage(QPoint((x * config->numBgTilesX() + bx) * bgTileImageSize.width(), (y * config->numBgTilesY() + by) * bgTileImageSize.height()), *image.get());
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
void OriginalTilesetScene::updateSelectionRect() {
  setSelection(mSelectionHelper.selection());
}

// -----------------------------------------------------------------------------
void OriginalTilesetScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {
  if (event->button() == Qt::RightButton) {
    mSelectionHelper.cancel();
    updateSelectionRect();
  } else if (event->button() == Qt::LeftButton) {
    mSelectionHelper.startSelection(event->scenePos());
    updateSelectionRect();
    QGraphicsScene::mousePressEvent(event);
  } else {
    QGraphicsScene::mousePressEvent(event);
  }
}

// -----------------------------------------------------------------------------
void OriginalTilesetScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  if (event->buttons() & Qt::LeftButton) {
    mSelectionHelper.moveSelection(event->scenePos());
    updateSelectionRect();
  }
}

// -----------------------------------------------------------------------------
void OriginalTilesetScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  mSelectionHelper.stopSelection(event->scenePos());
  updateSelectionRect();
	QGraphicsScene::mouseReleaseEvent(event);
}