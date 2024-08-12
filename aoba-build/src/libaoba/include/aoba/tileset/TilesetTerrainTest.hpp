#pragma once

#include <yaml-cpp/yaml.h>

namespace Aoba {

class TilesetTerrainRule;
class TilemapTerrainLayer;
struct TilemapTerrainBuildContext;

class TilesetTerrainTest {
public:
  //! Constructor
  TilesetTerrainTest(TilesetTerrainRule *rule) : mRule(rule) {}

  //! Deconstructor
  virtual ~TilesetTerrainTest() = default;

  //! Returns the type
  virtual QString typeId() const = 0;

  //! Applies the terrain
  virtual bool test(TilemapTerrainLayer*, TilemapTerrainBuildContext &context, const QPoint &pos) const = 0;
  
  //! Loads the terrain
  virtual bool load(const YAML::Node &) = 0;

  //! Saves the terrain
  virtual bool save(YAML::Emitter &) const = 0;

protected:
  //! The rule
  TilesetTerrainRule *mRule;
};

}