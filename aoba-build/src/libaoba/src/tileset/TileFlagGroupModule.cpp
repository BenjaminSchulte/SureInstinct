#include <QDebug>
#include <aoba/log/Log.hpp>
#include <aoba/csv/CsvReader.hpp>
#include <aoba/csv/CsvWriter.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/tileset/TileFlagGroupConfig.hpp>
#include <aoba/tileset/TileFlagGroupModule.hpp>
#include <aoba/compress/Lz77Writer.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool TileFlagGroupModuleInstance::loadTile(TilesetLayer *layer, TilesetTile *tile, CsvReader &csv) {
  TileFlagGroupTile ourTile;
  if (!groupModule()->group()->loadTile(layer, tile, ourTile, csv)) {
    return false;
  }
  mTiles[tile] = ourTile;
  return true;
}

// -----------------------------------------------------------------------------
QString TileFlagGroupModuleInstance::hashTile(TilesetLayer *layer, const TilesetTile *tile) {
  return groupModule()->group()->hashTile(layer, tile, mTiles[tile]);
}

// -----------------------------------------------------------------------------
void TileFlagGroupModuleInstance::writeCsvHeader(CsvWriter &csv) const {
  return groupModule()->group()->writeCsvHeader(csv);
}

// -----------------------------------------------------------------------------
void TileFlagGroupModuleInstance::writeTile(const TilesetLayer *layer, const TilesetTile *tile, CsvWriter &csv) const {
  groupModule()->group()->writeTile(layer, tile, mTiles[tile], csv);
}

// -----------------------------------------------------------------------------
void TileFlagGroupModuleInstance::replaceOldTiles(const QMap<const Aoba::TilesetTile*, Aoba::TilesetTile*> &map) {
  QMapIterator<const Aoba::TilesetTile*, Aoba::TilesetTile*> it(map);
  while (it.hasNext()) {
    it.next();
    mTiles[it.value()] = mTiles[it.key()];
  }
}

// -----------------------------------------------------------------------------
uint64_t TileFlagGroupModuleInstance::calculateTile(const TilesetLayer *, const TilesetTile *tile, bool &ok) const {
  return groupModule()->group()->generate(mTiles[tile], ok);
}

// -----------------------------------------------------------------------------
bool TileFlagGroupModuleInstance::build() {
  uint8_t bitDataSize = groupModule()->group()->dataSize();
  uint8_t byteDataSize = (bitDataSize + 7) / 8;

  for (TilesetAssetSetLayer *layerConfig : mTileset->assetSet()->layer()) {
    TilesetLayer *layer = mTileset->layer(layerConfig);

    int numTiles = layer->usedTiles().count();
    QByteArray layerData(numTiles * byteDataSize, 0);
    for (int i=0; i<numTiles; i++) {
      TilesetTile *tile = layer->usedTiles()[i];
      bool ok = true;
      uint64_t value = groupModule()->group()->generate(mTiles[tile], ok);
      if (!ok) {
        return false;
      }

      int index = i * byteDataSize;
      for (int j=0; j<bitDataSize; j+=8) {
        layerData.data()[index++] = value >> j;
      }
    }

    QString symbolName = mTileset->assetSymbolName() + "_tileflaggroup_" + layerConfig->id();
    FMA::linker::LinkerBlock *block = mTileset->assetSet()->assetLinkerObject().createLinkerBlock(symbolName);

    Lz77Writer compressor(mTileset->assetSet()->project());
    if (!compressor.writeArray(layerData) || !compressor.finalize()) {
      Aoba::log::error("Unable to compress tile flag group data " + mTileset->id());
      return false;
    }

    const QByteArray &buffer = compressor.buffer();
    block->write(buffer.data(), buffer.size());

    mTileset->headerBlock()->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(symbolName.toStdString())), 3);
  }

  return true;
}

// -----------------------------------------------------------------------------
QString TileFlagGroupModuleInstance::getFmaCode() const {
  Aoba::log::error("IS THIS BEING CALLED??????????????");
  return "";
}
