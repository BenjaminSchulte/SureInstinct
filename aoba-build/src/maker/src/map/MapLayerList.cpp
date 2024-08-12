#include <aoba/log/Log.hpp>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <maker/map/MapSceneLayerList.hpp>
#include <maker/map/MapSceneLayer.hpp>
#include <maker/map/MapLayerList.hpp>

// -----------------------------------------------------------------------------
MapLayerList::MapLayerList(QWidget *parent)
  : QTableWidget(parent)
{
  setColumnCount(3);
  setColumnHidden(2, true);

  horizontalHeader()->hide();
  horizontalHeader()->setStretchLastSection(true);
  verticalHeader()->hide();
  setSelectionBehavior(QAbstractItemView::SelectRows);
  setSelectionMode(QAbstractItemView::SingleSelection);
  setColumnWidth(0, 32);
  setEditTriggers(QAbstractItemView::NoEditTriggers);

  connect(&mVisibleMapper, SIGNAL(mapped(int)), this, SLOT(visibilityChanged(int)));
  connect(this, &MapLayerList::itemSelectionChanged, this, &MapLayerList::selectedRowChanged);
}

// -----------------------------------------------------------------------------
void MapLayerList::selectedRowChanged() {
  if (selectedRanges().isEmpty()) {
    return;
  }

  int row = selectedRanges().front().topRow();
  MapSceneLayer *layer = layerFromRow(row);

  emit layerChanged(layer);
}

// -----------------------------------------------------------------------------
void MapLayerList::removeLayers() {
  for (MapSceneLayer *layer : mLayers) {
    disconnect(layer);
  }

  mLayers.clear();
  mVisibleCheckBoxes.clear();
  clear();
  setRowCount(0);
}

// -----------------------------------------------------------------------------
void MapLayerList::addLayer(MapSceneLayer *layer) {
  int row = 0;

  insertRow(row);

  mLayers.push_back(layer);

  QWidget *widget = new QWidget(this);
  QHBoxLayout *layoutCheckBox = new QHBoxLayout(widget);
  QCheckBox *visible = new QCheckBox(this);
  widget->setLayout(layoutCheckBox);
  layoutCheckBox->addWidget(visible);
  visible->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  layoutCheckBox->setContentsMargins(0, 0, 0, 0);
  visible->setStyleSheet(
    "QCheckBox::indicator{width: 32px; height: " + QString::number(verticalHeader()->defaultSectionSize()) + "px; font-size:16px} "
    + "QCheckBox::indicator:checked {image: url(:/bright/view);}"
    + "QCheckBox::indicator:checked:pressed {background: rgba(0,0,0,0.15); image: url(:/bright/view);}"
  );
  visible->setChecked(layer->isVisible());
  mVisibleMapper.setMapping(visible, layer->id());
  connect(visible, SIGNAL(toggled(bool)), &mVisibleMapper, SLOT(map()));
  setCellWidget(row, 0, widget);
  setItem(row, 1, new QTableWidgetItem(layer->name()));
  setItem(row, 2, new QTableWidgetItem(QString::number(layer->id())));

  mVisibleCheckBoxes.insert(layer->id(), visible);
}

// -----------------------------------------------------------------------------
void MapLayerList::visibilityChanged(int id) {
  int row = rowFromId(id);
  MapSceneLayer *layer = layerFromRow(row);
  QCheckBox *visible = mVisibleCheckBoxes[id];
  if (!layer || !visible) {
    return;
  }

  layer->setVisible(visible->isChecked());
}

// -----------------------------------------------------------------------------
void MapLayerList::setLayerList(MapSceneLayerList *list) {
  removeLayers();

  if (list == nullptr) {
    return;
  }

  for (MapSceneLayer *layer : list->all()) {
    addLayer(layer);
  }
}

// -----------------------------------------------------------------------------
void MapLayerList::setSelectedLayer(MapSceneLayer *layer) {
  if (!layer) {
    return;
  }

  int row = rowFromId(layer->id());
  selectRow(row);
}

// -----------------------------------------------------------------------------
void MapLayerList::selectTilemapLayer(Aoba::TilemapAssetSetLayer *tilemapLayer) {
  if (currentRow() >= 0 && layerFromRow(currentRow())->layer() == tilemapLayer) {
    return;
  }

  for (MapSceneLayer *layer : mLayers) {
    if (layer->layer() == tilemapLayer) {
      setSelectedLayer(layer);
      break;
    }
  }
}

// -----------------------------------------------------------------------------
int MapLayerList::rowFromId(int id) const {
  for (int y=0; y<rowCount(); y++) {
    if (item(y, 2)->text().toInt() == id) {
      return y;
    }
  }

  return -1;
}

// -----------------------------------------------------------------------------
MapSceneLayer *MapLayerList::layerFromRow(int row) const {
  if (row < 0 || row >= rowCount()) {
    return nullptr;
  }

  auto *rowItem = item(row, 2);
  if (rowItem == nullptr) {
    return nullptr;
  }

  int id = rowItem->text().toInt();
  for (MapSceneLayer *layer : mLayers) {
    if (layer->id() == id) {
      return layer;
    }
  }

  return nullptr;
}