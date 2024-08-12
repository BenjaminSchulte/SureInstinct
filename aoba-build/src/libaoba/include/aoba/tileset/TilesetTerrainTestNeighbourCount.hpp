#pragma once

#include <QVector>
#include "TilesetTerrainTestTag.hpp"

namespace Aoba {

class TilesetTerrainTestNeighbourCount : public TilesetTerrainTestTag {
public:
  //! Constructor
  TilesetTerrainTestNeighbourCount(TilesetTerrainRule *rule) : TilesetTerrainTestTag(rule) {}

  //! Returns the type
  static QString TypeId() { return "neighbour_count"; }

  //! Returns the type
  QString typeId() const override { return TypeId(); }

  //! Applies the terrain
  bool test(TilemapTerrainLayer*, TilemapTerrainBuildContext &context, const QPoint &pos) const override;

  //! Loads the terrain
  bool load(const YAML::Node &) override;

  //! Saves the terrain
  bool save(YAML::Emitter &) const override;

private:
  //! The minimum count
  int mMinimum = 0;

  //! The maximum count
  int mMaximum = 4;

  //! The distance
  int mDistance = 1;
};

}