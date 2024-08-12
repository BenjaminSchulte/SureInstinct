#pragma once

#include <QVector>
#include "TilesetTerrainAction.hpp"

namespace Aoba {

class TilesetTerrainActionRandom : public TilesetTerrainAction {
public:
  //! Constructor
  TilesetTerrainActionRandom(TilesetTerrainRule *rule)
    : TilesetTerrainAction(rule) {}

  //! Deconstructor
  ~TilesetTerrainActionRandom();

  //! Clears the type
  void clear();

  //! Returns the type
  static QString TypeId() { return "random"; }

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

  //! List of sub rules
  QVector<TilesetTerrainAction*> mActions;
};

}
