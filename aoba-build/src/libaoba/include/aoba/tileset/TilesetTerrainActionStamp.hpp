#pragma once

#include <QSize>
#include <QPoint>
#include <QVector>
#include "TilesetTerrainAction.hpp"
#include "TilesetTerrainStamp.hpp"
#include "TilesetTileRef.hpp"

namespace Aoba {

class TilesetTerrainActionStamp : public TilesetTerrainAction {
public:
  //! Constructor
  TilesetTerrainActionStamp(TilesetTerrainRule *rule)
    : TilesetTerrainAction(rule) {}

  //! Returns the type
  static QString TypeId() { return "stamp"; }

  //! Returns the type
  QString typeId() const override { return TypeId(); }
    
  //! Applies the terrain
  void apply(TilemapTerrainLayer*, TilemapTerrainBuildContext &context, const QPoint &pos) const override;

  //! Loads the terrain
  bool load(const YAML::Node &) override;

  //! Saves the terrain
  bool save(YAML::Emitter &) const override;

private:
  //! The stamp
  TilesetTerrainStamp mStamp;

  //! Last
  bool mLast = false;
};

}
