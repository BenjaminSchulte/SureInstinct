#include <QFile>
#include <aoba/log/Log.hpp>
#include <aoba/csv/CsvReader.hpp>
#include <aoba/csv/CsvWriter.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/tileset/TilesetTile.hpp>
#include <aoba/compress/Lz77Writer.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <rpg/tileset/TilesetCollisionModule.hpp>

using namespace Aoba;
using namespace Rpg;

namespace {
  template<typename K, typename V>
  K findKeyOfValue(const QMap<K,V> &map, const V &value, const K &fallback) {
    QMapIterator<K,V> it(map);
    while (it.hasNext()) {
      it.next();
      if (it.value() == value) {
        return it.key();
      }
    }
    return fallback;
  }
}


// -----------------------------------------------------------------------------
bool TilesetTileCollision::hasDiagonalCollision() const {
  switch (onMove) {
    case DIAGONAL_COLLISION_NW:
    case DIAGONAL_COLLISION_NE:
    case DIAGONAL_COLLISION_SW:
    case DIAGONAL_COLLISION_SE:
      return true;

    default:
      return false;
  }
}

// -----------------------------------------------------------------------------
TilesetTileCollision TilesetTileCollision::flipped(bool x, bool y) const {
  TilesetTileCollision copy(*this);
  bool temp;

  if (hasDiagonalCollision()) {
    if (x) { copy.onMove = (MoveEffect)(copy.onMove ^ 1); }
    if (y) { copy.onMove = (MoveEffect)(copy.onMove ^ 2); }
  }

  if (x) {
    temp = copy.collision[0];
    copy.collision[0] = copy.collision[1];
    copy.collision[1] = temp;
    temp = copy.collision[2];
    copy.collision[2] = copy.collision[3];
    copy.collision[3] = temp;
  }

  if (x) {
    temp = copy.collision[0];
    copy.collision[0] = copy.collision[2];
    copy.collision[2] = temp;
    temp = copy.collision[1];
    copy.collision[1] = copy.collision[3];
    copy.collision[3] = temp;
  }

  return copy;
}

// -----------------------------------------------------------------------------
TilesetTileCollision::Outline TilesetTileCollision::oppositeOutline(Outline outline) {
  return (Outline)(outline ^ 2);
}

// -----------------------------------------------------------------------------
bool TilesetTileCollision::collidesAt(Outline outline) const {
  Edge edge;

  switch (outline) {
    case OUTLINE_TOP_LEFT: edge = NORTHWEST; break;
    case OUTLINE_LEFT_TOP: edge = NORTHWEST; break;
    case OUTLINE_TOP_RIGHT: edge = NORTHEAST; break;
    case OUTLINE_RIGHT_TOP: edge = NORTHEAST; break;
    case OUTLINE_BOTTOM_LEFT: edge = SOUTHWEST; break;
    case OUTLINE_LEFT_BOTTOM: edge = SOUTHWEST; break;
    case OUTLINE_BOTTOM_RIGHT: edge = SOUTHEAST; break;
    case OUTLINE_RIGHT_BOTTOM: edge = SOUTHEAST; break;
    default: edge = NORTHEAST; break;
  }

  if (collision[edge]) {
    return true;
  }

  bool diagonal = false;
  switch (outline) {
    case OUTLINE_TOP_LEFT: diagonal = (onMove == DIAGONAL_COLLISION_NW || onMove == DIAGONAL_COLLISION_NE); break;
    case OUTLINE_TOP_RIGHT: diagonal = (onMove == DIAGONAL_COLLISION_NW || onMove == DIAGONAL_COLLISION_NE); break;
    case OUTLINE_LEFT_TOP: diagonal = (onMove == DIAGONAL_COLLISION_NW || onMove == DIAGONAL_COLLISION_SW); break;
    case OUTLINE_LEFT_BOTTOM: diagonal = (onMove == DIAGONAL_COLLISION_NW || onMove == DIAGONAL_COLLISION_SW); break;
    case OUTLINE_RIGHT_TOP: diagonal = (onMove == DIAGONAL_COLLISION_NE || onMove == DIAGONAL_COLLISION_SE); break;
    case OUTLINE_RIGHT_BOTTOM: diagonal = (onMove == DIAGONAL_COLLISION_NE || onMove == DIAGONAL_COLLISION_SE); break;
    case OUTLINE_BOTTOM_LEFT: diagonal = (onMove == DIAGONAL_COLLISION_SW || onMove == DIAGONAL_COLLISION_SE); break;
    case OUTLINE_BOTTOM_RIGHT: diagonal = (onMove == DIAGONAL_COLLISION_SW || onMove == DIAGONAL_COLLISION_SE); break;
    default: break;
  }

  if (diagonal) {
    return true;
  }

  return false;
}

// -----------------------------------------------------------------------------
bool TilesetTileCollision::endsCollisionAt(Outline outline, const TilesetTileCollision &other) const {
  if (collidesAt(outline) && !other.collidesAt(oppositeOutline(outline))) {
    return true;
  }

  return false;
}


// -----------------------------------------------------------------------------
bool TilesetCollisionModuleInstance::loadTile(TilesetLayer*, TilesetTile *tile, CsvReader &csv) {
  TilesetTileCollision collision;

  collision.layer[0] = csv.get("layer0").toInt();
  collision.layer[1] = csv.get("layer1").toInt();
  collision.exit = csv.get("exit").toInt();
  collision.collision[TilesetTileCollision::NORTHWEST] = csv.get("nw").toInt();
  collision.collision[TilesetTileCollision::NORTHEAST] = csv.get("ne").toInt();
  collision.collision[TilesetTileCollision::SOUTHWEST] = csv.get("sw").toInt();
  collision.collision[TilesetTileCollision::SOUTHEAST] = csv.get("se").toInt();
  
  QString enter = csv.get("event");
  if (!enter.isEmpty()) {
    if (!collisionModule()->events().contains(enter)) {
      Aoba::log::warn("Could not find enter event: " + enter);
      return false;
    }
    collision.onEnter = (TilesetTileCollision::EnterEffect)collisionModule()->events()[enter];
  }

  QString move = csv.get("movement");
  if (!move.isEmpty()) {
    if (!collisionModule()->movements().contains(move)) {
      Aoba::log::warn("Could not find move event: " + move);
      return false;
    }
    collision.onMove = (TilesetTileCollision::MoveEffect)collisionModule()->movements()[move];
  }

  mCollision.insert(tile, collision);

  return true;
}

// -----------------------------------------------------------------------------
QString TilesetCollisionModuleInstance::hashTile(Aoba::TilesetLayer*, const Aoba::TilesetTile *tile) {
  return QString::number(collisionModule()->toInt(mCollision[tile]));
}

// -----------------------------------------------------------------------------
void TilesetCollisionModuleInstance::writeCsvHeader(CsvWriter &csv) const {
  csv << "nw" << "ne" << "sw" << "se";
  csv << "layer0" << "layer1" << "exit" << "event" << "movement";
}

// -----------------------------------------------------------------------------
void TilesetCollisionModuleInstance::writeTile(const Aoba::TilesetLayer*, const Aoba::TilesetTile *tile, Aoba::CsvWriter &csv) const { 
  const auto &col = forTile(tile);

  csv << (int)col.collision[0];
  csv << (int)col.collision[1];
  csv << (int)col.collision[2];
  csv << (int)col.collision[3];
  csv << (int)col.layer[0];
  csv << (int)col.layer[1];
  csv << (int)col.exit;
  csv << collisionModule()->eventToString(col.onEnter);
  csv << collisionModule()->movementToString(col.onMove);
}

// -----------------------------------------------------------------------------
uint16_t TilesetCollisionModule::toInt(const TilesetTileCollision &c) const {
  uint16_t value = 0;

  if (c.layer[0]) { value |= (int)TilesetCollisionValue::LAYER0; }
  if (c.layer[1]) { value |= (int)TilesetCollisionValue::LAYER1; }

  if (c.hasCollision()) {
    value |= (int)TilesetCollisionValue::COLLISION;
    value |= (c.collision[TilesetTileCollision::NORTHWEST] ? 1 : 0);
    value |= (c.collision[TilesetTileCollision::NORTHEAST] ? 2 : 0);
    value |= (c.collision[TilesetTileCollision::SOUTHWEST] ? 4 : 0);
    value |= (c.collision[TilesetTileCollision::SOUTHEAST] ? 8 : 0);
  } else {
    value |= (int)c.onMove;
  }

  if (c.exit) {
    value |= (int)TilesetCollisionValue::EXIT;
    value &= (int)TilesetCollisionValue::EXIT_MASK;
  }

  value |= (int)c.onEnter << 11;

  return value;
}

// -----------------------------------------------------------------------------
bool TilesetCollisionModuleInstance::build() {
  for (TilesetAssetSetLayer *layerConfig : mTileset->assetSet()->layer()) {
    TilesetLayer *layer = mTileset->layer(layerConfig);

    int numTiles = layer->usedTiles().count();
    QByteArray layerData(numTiles * 2, 0);
    for (int i=0; i<numTiles; i++) {
      TilesetTile *tile = layer->usedTiles()[i];
      uint16_t value = collisionModule()->toInt(mCollision[tile]);
      layerData.data()[i * 2 + 0] = value & 0xFF;
      layerData.data()[i * 2 + 1] = value >> 8;
    }

    QString symbolName = mTileset->assetSymbolName() + "_collision_layer_" + layerConfig->id();
    FMA::linker::LinkerBlock *block = mTileset->assetSet()->assetLinkerObject().createLinkerBlock(symbolName);

    Lz77Writer compressor(mTileset->assetSet()->project());
    if (!compressor.writeArray(layerData) || !compressor.finalize()) {
      Aoba::log::warn("Unable to compress collision data " + mTileset->id());
      return false;
    }

    const QByteArray &buffer = compressor.buffer();
    block->write(buffer.data(), buffer.size());

    mTileset->headerBlock()->write(FMA::symbol::SymbolReferencePtr(new FMA::symbol::SymbolReference(symbolName.toStdString())), 3);
  }

  return true;
}

// -----------------------------------------------------------------------------
QString TilesetCollisionModuleInstance::getFmaCode() const {
  QStringList layers;
  Aoba::log::error("IS THIS BEING CALLED??????????????");

  for (TilesetAssetSetLayer *layerConfig : mTileset->assetSet()->layer()) {
    TilesetLayer *layer = mTileset->layer(layerConfig);
    QString fileName(layer->binaryFileName("collision.bin"));

    layers << collisionModule()->assetName() + ".data[:" + layerConfig->id() + "].load_from_file \"" + fileName + "\"";
  }
  
  return layers.join('\n');
}

// -----------------------------------------------------------------------------
QString TilesetCollisionModule::eventToString(int event) const {
  return findKeyOfValue<QString, int>(mEvents, event, "");
}

// -----------------------------------------------------------------------------
QString TilesetCollisionModule::movementToString(int event) const {
  return findKeyOfValue<QString, int>(mMovements, event, "");
}