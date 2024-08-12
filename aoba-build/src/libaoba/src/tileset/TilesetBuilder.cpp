#include <aoba/log/Log.hpp>
#include <aoba/image/ImageCollection.hpp>
#include <aoba/image/PaletteImageAccessor.hpp>
#include <aoba/image/SnesPaletteImage.hpp>
#include <aoba/palette/SnesPalette.hpp>
#include <aoba/palette/Palette.hpp>
#include <aoba/palette/PaletteGroup.hpp>
#include <aoba/tileset/BgTile.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/tileset/TilesetTile.hpp>
#include <aoba/tileset/TilesetBuilder.hpp>
#include <aoba/compress/BgTileWriter.hpp>
#include <aoba/compress/Lz77Writer.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <fma/symbol/CalculatedNumber.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool TilesetBuilder::build() {
  for (TilesetAssetSetTiles *tiles : mTileset->assetSet()->tiles()) {
    if (!buildTiles(tiles)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TilesetBuilder::buildTiles(TilesetAssetSetTiles *tiles) {
  ImageCollection collection;

  for (TilesetAssetSetLayer *layer : mTileset->assetSet()->layer()) {
    if (layer->tiles() != tiles) {
      continue;
    }

    //Aoba::log::debug(".. Building tile layer " + layer->id());
    if (!buildAnimatedTileData(collection, layer)) {
      return false;
    }
  }

  for (TilesetAssetSetLayer *layer : mTileset->assetSet()->layer()) {
    if (layer->tiles() != tiles) {
      continue;
    }

    //Aoba::log::debug(".. Building tile layer " + layer->id());
    if (!buildTileData(collection, tiles, layer)) {
      return false;
    }
  }

  //Aoba::log::debug(".. Building tile image " + tiles->id());
  if (!buildTileImage(collection, tiles)) {
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TilesetBuilder::buildTileImage(ImageCollection &collection, TilesetAssetSetTiles *tilesConfig) {
  if (collection.numImages() == 0) {
    Aoba::log::warn("No tiles have been generated for tileset " + mTileset->id());
    return false;
  }

  TilesetTiles *tiles = mTileset->tiles(tilesConfig);
  SnesPaletteImage image(collection.join()->toPaletteImage());

  Lz77Writer compressor(mTileset->assetSet()->project(), mTileset->assetSet()->maxBytesPerCompressionChunk());
  uint16_t colorOffset = tilesConfig->paletteGroupSet()->paletteSet()->colorOffset();
  if (!compressor.writeArray(image.compile(mTileset->assetSet()->bgTileWidth(), mTileset->assetSet()->bgTileHeight(), colorOffset)) || !compressor.finalize()) {
    Aoba::log::warn("Unable to compress image for tileset " + mTileset->id());
    return false;
  }

  if ((int)collection.numImages() > tilesConfig->maxTiles()) {
    Aoba::log::warn("Tileset " + mTileset->id() + " contains too many bg-tiles. Maximum is " + QString::number(tilesConfig->maxTiles()) + " got " + QString::number(collection.numImages()));
    return false;
  }

  FMA::linker::LinkerBlock *tilesBlock = mTileset->assetSet()->assetLinkerObject().createLinkerBlock(tiles->imageBinaryFileName());
  QByteArray data(compressor.buffer());
  tilesBlock->write(data.data(), data.size());

  mTileset->headerBlock()->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(tiles->imageBinaryFileName().toStdString())), 3);

  return true;
}

// -----------------------------------------------------------------------------
QString TilesetBuilder::hash(const QString &id, int row, int partIndex) const {
  return id + "#" + QString::number(row) + "#" + QString::number(partIndex);
}

// -----------------------------------------------------------------------------
QString TilesetBuilder::hash(const TilesetTilePart &part, int partIndex) const {
  return hash(part.animationId(), part.animationRow(), partIndex);
}

// -----------------------------------------------------------------------------
bool TilesetBuilder::buildAnimatedTileData(ImageCollection &collection, TilesetAssetSetLayer *layerConfig) {
  TilesetLayer *layer = mTileset->layer(layerConfig);

  QVector<TilesetTile*> usedTiles = layer->usedTiles();
  std::sort(usedTiles.begin(), usedTiles.end(), [](TilesetTile *left, TilesetTile *right) {
    return left->tileLayer(0)->part(0).animationId() < right->tileLayer(0)->part(0).animationId();
  });

  int numBgTilesX = mTileset->assetSet()->numBgTilesX();
  int numBgTilesY = mTileset->assetSet()->numBgTilesY();

  struct GroupList {
    GroupList() {}
    GroupList(const TilesetTile *tile, uint8_t bg, uint partIndex) : tile(tile), bg(bg), partIndex(partIndex) {}
    
    const TilesetTile *tile;
    uint8_t bg;
    uint partIndex;
  };

  QMap<QString, QVector<GroupList>> listByGroup;

  for (const auto *tile : usedTiles) {
    for (uint8_t bg=0; bg<layer->config()->numBackgrounds(); bg++) {
      for (uint partIndex=0; partIndex<tile->tileLayer(bg)->numParts(); partIndex++) {
        const auto &part = tile->tileLayer(bg)->constPart(partIndex);
        if (!part.isAnimatedTile()) {
          continue;
        }

        listByGroup[part.animationId()].push_back(GroupList(tile, bg, partIndex));
      }
    }
  }

  QMapIterator<QString, QVector<GroupList>> it(listByGroup);
  while (it.hasNext()) {
    it.next();

    for (const auto &item : it.value()) {
      const auto &part = item.tile->tileLayer(item.bg)->constPart(item.partIndex);

      QString tileHash(hash(part, item.partIndex));
      if (mAnimatedTiles.contains(tileHash)) {
        continue;
      }


      TilesetAnimationGroup *animationGroup = mTileset->animation(part.animationId());
      int firstFrameId = animationGroup->frame(0).imageIndex();
      int inFrameIndex = part.animationRow() * numBgTilesX * numBgTilesY + item.partIndex;
      ImageAccessorPtr image;
      
      if (firstFrameId == -1) {
        QByteArray temp;
        temp.resize(16 * 16);
        temp.fill(0);
        image = ImageAccessorPtr(new PaletteImageAccessor(PaletteImagePtr(new PaletteImage(temp, 16, 16, 16))));
      } else {
        const TilesetAnimationGroupImage &animationGroupImage = animationGroup->image(firstFrameId);
        image = ImageAccessorPtr(new PaletteImageAccessor(animationGroupImage.tiles()[inFrameIndex]));
      }
  
      auto ptr = collection.addAlways(image, true);

      auto &animation = mTileset->generatedAnimations()[part.animationId()];
      animation.group = animationGroup;
      animation.firstIndex = qMin<int>(ptr.index, animation.firstIndex);
      animation.lastIndex = qMax<int>(ptr.index, animation.lastIndex);
      animation.indices.insert(ptr.index, inFrameIndex);

      mAnimatedTiles.insert(tileHash, ptr);
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool TilesetBuilder::buildTileData(ImageCollection &collection, TilesetAssetSetTiles *tilesConfig, TilesetAssetSetLayer *layerConfig) {
  TilesetLayer *layer = mTileset->layer(layerConfig);
  TilesetTiles *tiles = mTileset->tiles(tilesConfig);

  QVector<BgTile> bgTiles;
  uint32_t bgTilesPerTileX = mTileset->assetSet()->numBgTilesX();
  uint32_t bgTilesPerTileY = mTileset->assetSet()->numBgTilesY();

  for (const TilesetTile *tile : layer->usedTiles()) {
    for (uint8_t bg=0; bg<layer->config()->numBackgrounds(); bg++) {
      for (uint partIndex=0; partIndex<tile->tileLayer(bg)->numParts(); partIndex++) {
        const auto &part = tile->tileLayer(bg)->constPart(partIndex);

        ImageCollectionPointer ptr;

        if (part.isAnimatedTile()) {
          ptr = mAnimatedTiles[hash(part, partIndex)];
        } else {
          ImageAccessorPtr img(new PaletteImageAccessor(tiles->bgTiles()[part.bgTileIndex()].data));
          ptr = collection.add(img);
        }

        ptr.index *= mTileset->assetSet()->tileIndexMultiplier();
        BgTile bgTile(ptr.index, part.paletteIndex() + tiles->config()->paletteBase());

        bgTile.priority = part.priority();
        bgTile.flipX = ptr.mirrorX == !part.mirrorX();
        bgTile.flipY = ptr.mirrorY == !part.mirrorY();
        bgTiles.push_back(bgTile);

        tiles->addBgTileMapping(part.bgTileIndex(), ptr);
      }
    }
  }

  int actualTiles = bgTiles.count() / bgTilesPerTileY / bgTilesPerTileX;
  if (actualTiles > (int)layer->config()->maxTiles()) {
    Aoba::log::warn("Tileset index too large. Maximum of " + QString::number(layer->config()->maxTiles()) + " exceeded. Got " + QString::number(actualTiles));
    return false;
  }

  if (layerConfig->isTileIndexMode()) {
    FMA::linker::LinkerBlock *indexBlock = mTileset->assetSet()->assetLinkerObject().createLinkerBlock(layer->indexBinaryFileName());

    if (mTileset->assetSet()->indexCompressed()) {
      BgTileWriter compressor(mTileset->assetSet()->project());
      if (!compressor.writeArray(BgTile::toSnesBgTiles(bgTiles)) || !compressor.finalize()) {
        Aoba::log::warn("Unable to compress tileset " + layer->config()->id());
        return false;
      }
      QByteArray data(compressor.buffer());
      indexBlock->write(data.data(), data.size());
    } else {
      QByteArray data(BgTile::toSnesBgTiles(bgTiles));
      indexBlock->write(data.data(), data.size());
    }

    mTileset->headerBlock()->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(layer->indexBinaryFileName().toStdString())), 3);
  }

  return true;
}
