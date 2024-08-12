#include <aoba/log/Log.hpp>
#include <aoba/csv/CsvReader.hpp>
#include <aoba/csv/CsvWriter.hpp>
#include <aoba/palette/SnesPalette.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/tileset/TilesetTile.hpp>
#include <aoba/tileset/TileBitplane2dModule.hpp>
#include <aoba/tileset/BgTileImageLoader.hpp>
#include <aoba/image/QImageAccessor.hpp>
#include <aoba/image/PaletteImageAccessor.hpp>
#include <aoba/image/ImageCollection.hpp>
#include <fma/output/DynamicBuffer.hpp>
#include <aoba/compress/Lz77Writer.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
TileBitplane2dModuleInstance::~TileBitplane2dModuleInstance() {
  for (ImageCollection *layer : mLayers) {
    delete layer;
  }

  delete mProperties;
}

// -----------------------------------------------------------------------------
bool TileBitplane2dModuleInstance::loadLayer(TilesetLayer *layer) {
  SnesPalettePtr palette(new SnesPalette(2));
  palette->color(0) = SnesColor(0, 0, 0, 0);
  palette->color(1) = SnesColor(255, 255, 255, 255);

  QString fileName = mTileset->path().absoluteFilePath(layer->config()->id() + "-" + bitplaneModule()->name() + ".png");
  QImagePtr tilesImage(QImageAccessor::loadImage(fileName));
  if (!tilesImage) {
    Aoba::log::error("Unable to load image: " + fileName);
    return false;
  }

  BgTileImageLoader loader(2, true, QSize(mTileset->assetSet()->tileWidth(), mTileset->assetSet()->tileHeight()));
  loader.setTargetPalette(palette);
  if (!loader.add(tilesImage)) {
    return false;
  }

  ImageCollection *collection = new ImageCollection(false);
  for (const TilesetBgTile &tile : loader.result()) {
    ImageAccessorPtr accessor(new PaletteImageAccessor(tile.data));
    mBitplaneToGenerated.push_back(collection->add(accessor).index);
  }

  mLayers.insert(layer, collection);

  return true;
}

// -----------------------------------------------------------------------------
bool TileBitplane2dModuleInstance::loadTile(TilesetLayer *layer, TilesetTile *tile, CsvReader &csv) {
  if (!mLayers.contains(layer) && !loadLayer(layer)) {
    return false;
  }

  int index = csv.get(bitplaneModule()->name()).toInt();
  if (index < 0 || index >= mBitplaneToGenerated.size()) {
    Aoba::log::error("Bitplane2d index out of range");
    return false;
  }

  mTileToBitplane.insert(tile, index);

  if (mProperties && !mProperties->loadTile(layer, tile, csv)) {
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
QString TileBitplane2dModuleInstance::hashTile(TilesetLayer *layer, const TilesetTile *tile) {
  return QString::number(mTileToBitplane[tile]) + (mProperties ? "#" + mProperties->hashTile(layer, tile) : "");
}

// -----------------------------------------------------------------------------
void TileBitplane2dModuleInstance::writeCsvHeader(CsvWriter &csv) const {
  csv << bitplaneModule()->name();
  
  if (mProperties) {
    mProperties->writeCsvHeader(csv);
  }
}

// -----------------------------------------------------------------------------
void TileBitplane2dModuleInstance::writeTile(const TilesetLayer *layer, const TilesetTile *tile, CsvWriter &csv) const {
  csv << QString::number(mTileToBitplane[tile]);
  
  if (mProperties) {
    mProperties->writeTile(layer, tile, csv);
  }
}

// -----------------------------------------------------------------------------
void TileBitplane2dModuleInstance::replaceOldTiles(const QMap<const Aoba::TilesetTile*, Aoba::TilesetTile*> &map) {
  QMapIterator<const Aoba::TilesetTile*, Aoba::TilesetTile*> it(map);
  while (it.hasNext()) {
    it.next();
    mTileToBitplane[it.value()] = mTileToBitplane[it.key()];
  }
  
  if (mProperties) {
    mProperties->replaceOldTiles(map);
  }
}

// -----------------------------------------------------------------------------
uint64_t TileBitplane2dModuleInstance::buildTileRow(const ImageAccessorPtr &image, uint32_t y) const {
  uint64_t bits = 0;
  uint8_t index = 0;

  for (uint32_t x=0; x<mTileset->assetSet()->tileWidth(); x++, index++) {
    if (!image->pixelIsTransparent(x, y)) {
      bits |= 1 << index;
    }
  }

  return bits;
}

// -----------------------------------------------------------------------------
bool TileBitplane2dModuleInstance::buildTile(FMA::output::DynamicBuffer &buffer, const ImageAccessorPtr &image) const {
  uint8_t bytesPerRow = (mTileset->assetSet()->tileWidth() + 7) / 8;

  for (uint32_t y=0; y<mTileset->assetSet()->tileHeight(); y++) {
    uint64_t row = buildTileRow(image, y);
    buffer.writeNumber(row, bytesPerRow);
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TileBitplane2dModuleInstance::build() {
  for (TilesetAssetSetLayer *layerConfig : mTileset->assetSet()->layer()) {
    TilesetLayer *layer = mTileset->layer(layerConfig);
    int numTiles = layer->usedTiles().count();

    uint32_t headerSize = numTiles * 2;

    FMA::output::DynamicBuffer header;
    FMA::output::DynamicBuffer bitplane;

    QMap<int, int> bitplaneIndexToOffset;

    for (int i=0; i<numTiles; i++) {
      TilesetTile *tile = layer->usedTiles()[i];
      int32_t bitplaneIndex = mBitplaneToGenerated[mTileToBitplane[tile]];

      uint16_t offset = 0;
      uint16_t headerData = 0;
     
      if (bitplaneIndex < 0) {
        offset = 0;
      } else {
        bool ok = true;
        if (mProperties) {
          headerData |= mProperties->calculateTile(layer, tile, ok);
          if (!ok) {
            return false;
          }
        }

        if (bitplaneIndexToOffset.contains(bitplaneIndex)) {
          offset = bitplaneIndexToOffset[bitplaneIndex];
        } else {
          offset = headerSize + bitplane.getSize();
          bitplaneIndexToOffset.insert(bitplaneIndex, offset);

          if (!buildTile(bitplane, mLayers[layer]->getImage(bitplaneIndex))) {
            return false;
          }
        }
      }

      headerData |= offset;
      header.writeNumber(headerData, 2);
    }

    header.write(bitplane.getData(), bitplane.getSize());
    if (header.getSize() > bitplaneModule()->maxSizeInBytes()) {
      Aoba::log::error("Bitplane data exceeded maximum size of " + QString::number(bitplaneModule()->maxSizeInBytes()) + ". Generated size is " + QString::number(header.getSize()) + ". Header size is " + QString::number(headerSize));
      return false;
    }

    Lz77Writer compressor(mTileset->assetSet()->project());
    if (!compressor.write((const char *)header.getData(), header.getSize()) || !compressor.finalize()) {
      Aoba::log::error("Unable to compress tile bitplane2d data " + mTileset->id());
      return false;
    }

    QString symbolName = mTileset->assetSymbolName() + "_bitplane_" + layerConfig->id();
    FMA::linker::LinkerBlock *block = mTileset->assetSet()->assetLinkerObject().createLinkerBlock(symbolName);
    const QByteArray &buffer = compressor.buffer();
    block->write(buffer.data(), buffer.size());
    mTileset->headerBlock()->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(symbolName.toStdString())), 3);
  }

  return true;
}

// -----------------------------------------------------------------------------
QString TileBitplane2dModuleInstance::getFmaCode() const {
  return "";
}