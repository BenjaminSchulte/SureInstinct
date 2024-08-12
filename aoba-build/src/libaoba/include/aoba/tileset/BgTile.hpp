#pragma once

#include <QString>
class QByteArray;

namespace Aoba {

struct BgTile {
  BgTile() : tile(0), priority(false), palette(0), flipX(false), flipY(false) {}
  BgTile(uint16_t tile, uint8_t palette) : tile(tile), priority(false), palette(palette), flipX(false), flipY(false) {}

  uint16_t tile;
  bool priority;
  uint8_t palette;
  bool flipX;
  bool flipY;

  uint16_t toSnesBgTile() const {
    return (tile & 0x3FF)
      | ((palette & 0x07) << 10)
      | (priority ? 0x2000 : 0)
      | (flipX ? 0x4000 : 0)
      | (flipY ? 0x8000 : 0)
    ;
  }

  static BgTile fromSnesBgTile(uint16_t value) {
    BgTile tile;
    tile.tile = value & 0x3FF;
    tile.palette = (value >> 10) & 0x07;
    tile.priority = value & 0x2000;
    tile.flipX = value & 0x4000;
    tile.flipY = value & 0x8000;
    return tile;
  }

  static QByteArray toSnesBgTiles(const QVector<BgTile> &);
};

}