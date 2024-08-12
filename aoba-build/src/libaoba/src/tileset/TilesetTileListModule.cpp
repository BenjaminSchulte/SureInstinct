#include <QDebug>
#include <aoba/log/Log.hpp>
#include <aoba/csv/CsvReader.hpp>
#include <aoba/csv/CsvWriter.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/tileset/TilesetTile.hpp>
#include <aoba/tileset/TilesetTileListModule.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool TilesetTileListModuleInstance::loadTile(TilesetLayer *layer, TilesetTile *tile, CsvReader &csv) {
  auto &list = mTiles[layer];

  QStringList offsets = csv.get(listModule()->name()).split(':');
  for (const QString &strOffset : offsets) {
    if (strOffset.isEmpty()) {
      continue;
    }

    int offset = strOffset.toInt();
    if (offset < 0) {
      continue;
    }

    while (list.size() <= offset) {
      list.push_back(TilesetTileRef());
    }
    list[offset] = layer->require(tile->originalIndex(), false);
  }
  return true;
}

// -----------------------------------------------------------------------------
QString TilesetTileListModuleInstance::hashTile(TilesetLayer *layer, const TilesetTile *tile) {
  (void)layer;
  (void)tile;
  return "";
}

// -----------------------------------------------------------------------------
void TilesetTileListModuleInstance::writeCsvHeader(CsvWriter &csv) const {
  csv << listModule()->name();
}

// -----------------------------------------------------------------------------
void TilesetTileListModuleInstance::writeTile(const TilesetLayer *layer, const TilesetTile *tile, CsvWriter &csv) const {
  QStringList result;

  int index = 0;
  for (const TilesetTileRef &ref : mTiles[layer]) {
    if (ref.originalIndex == tile->originalIndex()) {
      result.push_back(QString::number(index));
    }
    index++;
  }

  csv << result.join(':');
}

// -----------------------------------------------------------------------------
void TilesetTileListModuleInstance::replaceOldTiles(const QMap<const Aoba::TilesetTile*, Aoba::TilesetTile*> &map) {
  Aoba::log::error("TODO: TilesetTileListModuleInstance::replaceOldTiles");
  /*QMapIterator<const Aoba::TilesetTile*, Aoba::TilesetTile*> it(map);
  while (it.hasNext()) {
    it.next();
    mTiles[it.value()] = mTiles[it.key()];
  }*/
  (void)map;
}

// -----------------------------------------------------------------------------
bool TilesetTileListModuleInstance::build() {
  for (TilesetAssetSetLayer *layerConfig : mTileset->assetSet()->layer()) {
    TilesetLayer *layer = mTileset->layer(layerConfig);

    QString symbolName = mTileset->assetSymbolName() + "_tilelist_" + listModule()->name();
    FMA::linker::LinkerBlock *block = mTileset->assetSet()->assetLinkerObject().createLinkerBlock(symbolName, listModule()->listRomBank());
    for (const auto &tile : mTiles[layer]) {
      block->writeNumber(tile.index, listModule()->bytesPerTile());
    }

    mTileset->headerBlock()->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(symbolName.toStdString())), 2);
  }

  return true;
}

// -----------------------------------------------------------------------------
QString TilesetTileListModuleInstance::getFmaCode() const {
  return "";
}
