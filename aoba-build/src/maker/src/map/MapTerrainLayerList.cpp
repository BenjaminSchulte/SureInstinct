#include <QPixmap>
#include <QPainter>
#include <aoba/tileset/TilesetTerrainLayer.hpp>
#include <aoba/tileset/TilesetTerrainType.hpp>
#include <aoba/tileset/TilesetTerrain.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <maker/map/MapTerrainLayerList.hpp>
#include <maker/tileset/TileRenderCache.hpp>
#include <maker/GlobalContext.hpp>

// -----------------------------------------------------------------------------
MapTerrainLayerList::MapTerrainLayerList(QWidget *parent)
  : QListWidget(parent)
{
  setIconSize(QSize(32, 32));

  QFont copy(font());
  copy.setPixelSize(16);
  setFont(copy);

  mEmptyPixmap = QPixmap(32, 32);
  mEmptyPixmap.fill(QColor(0, 0, 0, 0));
  QPainter painter(&mEmptyPixmap);
  painter.setPen(QPen(QColor(255, 0, 0, 255), 2));
  painter.drawLine(QPoint(0, 0), QPoint(32, 32));
  painter.drawLine(QPoint(32, 0), QPoint(0, 32));


  connect(this, &QListWidget::currentRowChanged, this, &MapTerrainLayerList::setBrushFromCurrentRow);
}

// -----------------------------------------------------------------------------
void MapTerrainLayerList::setLayer(Aoba::TilesetTerrainLayer *layer) {
  mLayer = layer;

  recreateTerrainList();
}

// -----------------------------------------------------------------------------
int MapTerrainLayerList::brush() const {
  if (currentRow() == -1) {
    return -1;
  }

  QString key = item(currentRow())->data(Qt::UserRole).toString();
  if (key.isEmpty()) {
    return -1;
  }

  return mLayer->terrain()->type(key)->index();
}

// -----------------------------------------------------------------------------
void MapTerrainLayerList::setBrushFromCurrentRow() {
  emit terrainChanged(brush());
}

// -----------------------------------------------------------------------------
void MapTerrainLayerList::recreateTerrainList() {
  clear();

  if (!mLayer) {
    return;
  }

  Aoba::TilesetLayer *tilesetLayer = mLayer->defaultLayer();
  if (!tilesetLayer) {
    return;
  }

  QListWidgetItem *item = new QListWidgetItem(mEmptyPixmap, tr("- Empty -"));
  item->setData(Qt::UserRole, "");
  addItem(item);

  QMapIterator<QString, Aoba::TilesetTerrainType*> it(mLayer->terrain()->types());
  while (it.hasNext()) {
    it.next();

    if (!it.value()->layers().contains(mLayer)) {
      continue;
    }

    QPixmap pixmap(32, 32);
    pixmap.fill(QColor(0, 0, 0, 0));
    QPainter painter(&pixmap);

    const auto &cache = context().tileRenderCache()->get(tilesetLayer);
    if (it.value()->iconTile() < cache.backTiles.size()) {
      painter.drawPixmap(QRect(0, 0, 32, 32), cache.backTiles[it.value()->iconTile()]);
      painter.drawPixmap(QRect(0, 0, 32, 32), cache.frontTiles[it.value()->iconTile()]);
    }

    QListWidgetItem *item = new QListWidgetItem(QIcon(pixmap), it.value()->name());
    item->setData(Qt::UserRole, it.key());
    addItem(item);
  }

  setCurrentRow(0);
}
