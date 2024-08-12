#pragma once

#include "TilesetTerrainTest.hpp"

namespace Aoba {

class TilesetTerrainTestNoise : public TilesetTerrainTest {
public:
  //! Constructor
  TilesetTerrainTestNoise(TilesetTerrainRule *rule) : TilesetTerrainTest(rule) {}

  //! Returns the percentage
  inline float percentage() const { return mPercentage; }

  //! Returns the type
  static QString TypeId() { return "noise"; }

  //! Returns the type
  QString typeId() const override { return TypeId(); }
  
  //! Applies the terrain
  bool test(TilemapTerrainLayer*, TilemapTerrainBuildContext &context, const QPoint &pos) const override;
  
  //! Loads the terrain
  bool load(const YAML::Node &) override;

  //! Saves the terrain
  bool save(YAML::Emitter &) const override;

private:
  //! Seed
  int mSeed = 0;

  //! Percentage
  float mPercentage = 0.5;
};

}