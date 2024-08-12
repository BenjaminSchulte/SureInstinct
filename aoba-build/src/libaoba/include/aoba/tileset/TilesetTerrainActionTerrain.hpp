#pragma once

#include <QSize>
#include <QPoint>
#include <QVector>
#include "TilesetTerrainAction.hpp"
#include "TilesetTerrainStamp.hpp"
#include "TilesetTileRef.hpp"

namespace Aoba {

struct TilesetTerrainActionTerrainGroup {
  uint16_t mask = 0;
  uint16_t compare = 0;
  TilesetTerrainStamp stamp;
};

class TilesetTerrainActionTerrain : public TilesetTerrainAction {
public:
  //! Constructor
  TilesetTerrainActionTerrain(TilesetTerrainRule *rule)
    : TilesetTerrainAction(rule) {}

  //! Returns the type
  static QString TypeId() { return "terrain"; }

  //! Returns the type
  QString typeId() const override { return TypeId(); }
    
  //! Applies the terrain
  void apply(TilemapTerrainLayer*, TilemapTerrainBuildContext &context, const QPoint &pos) const override;

  //! Loads the terrain
  bool load(const YAML::Node &) override;

  //! Saves the terrain
  bool save(YAML::Emitter &) const override;

private:
  //! The seed
  int mSeed = 0;

  //! Last?
  bool mLast = false;

  //! List of all tags
  QVector<int> mTestTags;

  //! List of all groups
  QVector<TilesetTerrainActionTerrainGroup> mGroups;
};

}
