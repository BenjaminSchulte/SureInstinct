#include <QDebug>
#include <QBuffer>
#include <QByteArray>
#include <fma/linker/LinkerBlock.hpp>
#include <aoba/compress/MapTilesWriter.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/tilemap/Tilemap.hpp>
#include <aoba/tilemap/TilemapLayerBuilder.hpp>
#include <aoba/tileset/BgTile.hpp>
#include <aoba/tileset/TilesetTile.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool TilemapLayerBuilder::build(FMA::linker::LinkerBlock *block) {
  QByteArray result(generateMapData());

  QString compressorName = mLayer->tilemap()->assetSet()->compressor();
  Project *project = mLayer->tilemap()->assetSet()->project();
  AbstractWriter *writer = project->createCompressor(compressorName);
  if (writer == nullptr) {
    Aoba::log::error("No compressor registered named " + compressorName);
    return false;
  }

  if (!writer->writeArray(result) || !writer->finalize()) {
    Aoba::log::error("Unable to compress map tiles");
    delete writer;
    return false;
  }

  block->write(writer->buffer().data(), writer->buffer().size());
  delete writer;

  return true;
}

// -----------------------------------------------------------------------------
QByteArray TilemapLayerBuilder::generateMapData() {
  uint32_t numTiles = mLayer->tilemap()->width() * mLayer->tilemap()->height();

  QByteArray data;
  data.reserve(numTiles * 2 + 2);

  QBuffer buffer(&data);
  buffer.open(QIODevice::WriteOnly);

  // HEADER
  buffer.putChar(mLayer->tilemap()->width() - 1);
  buffer.putChar(mLayer->tilemap()->height() - 1);

  // DATA
  if (mLayer->config()->tilesetLayer()->isTileIndexMode()) {
    generateTileIndexMapData(buffer);
  } else {
    generateRawMapData(buffer);
  }

  return data;
}

// -----------------------------------------------------------------------------
void TilemapLayerBuilder::generateTileIndexMapData(QBuffer &buffer) {
  uint32_t width = mLayer->tilemap()->width();
  uint32_t height = mLayer->tilemap()->height();

  for (uint32_t y=0; y<height; y++) {
    for( uint32_t x=0; x<width; x++) {
      TilemapTile tile = mLayer->generatedTile(x, y);

      uint16_t value = tile.tile.index & 0x1FF;
      if (tile.tile.flipX) {
        value |= 0x8000;
      }
      if (tile.tile.flipY) {
        value |= 0x4000;
      }

      buffer.putChar(value);
      buffer.putChar(value >> 8);
    }
  }
}

// -----------------------------------------------------------------------------
void TilemapLayerBuilder::generateRawMapData(QBuffer &buffer) {
  TilesetLayer *tilesetLayer = mLayer->tilemap()->tileset()->layer(mLayer->config()->tilesetLayer());
  TilesetTiles *tilesetTiles = mLayer->tilemap()->tileset()->tiles(tilesetLayer->config()->tiles());

  for (uint32_t bg=0; bg<tilesetLayer->config()->numBackgrounds(); bg++) {
    for (uint32_t y=0; y<mLayer->tilemap()->height(); y++) {
      for (uint32_t x=0; x<mLayer->tilemap()->width(); x++) {
        TilemapTile tilemapTile = mLayer->generatedTile(x, y);
        TilesetTileLayer *tilesetTile = tilesetLayer->usedTiles()[tilemapTile.tile.index]->tileLayer(bg);

        for (unsigned int i=0; i<tilesetTile->numParts(); i++) {
          const TilesetTilePart &part = tilesetTile->constPart(i);

          const ImageCollectionPointer &ptr = tilesetTiles->mapGeneratedBgTile(part.bgTileIndex());
            
          BgTile bgTile(ptr.index + tilesetTiles->tileOffset(), part.paletteIndex() + tilesetTiles->config()->paletteBase());
          bgTile.priority = part.priority();
          bgTile.flipX = ptr.mirrorX ^ part.mirrorX() ^ tilemapTile.tile.flipX;
          bgTile.flipY = ptr.mirrorY ^ part.mirrorY() ^ tilemapTile.tile.flipY;

          uint16_t snesTile = bgTile.toSnesBgTile();
          buffer.putChar(snesTile);
          buffer.putChar(snesTile >> 8);
        }
      }
    }
  }
}