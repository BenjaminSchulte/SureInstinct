#pragma once

#include <yaml-cpp/yaml.h>

namespace Aoba {

class TilesetTerrainRule;
class TilemapTerrainLayer;
struct TilemapTerrainBuildContext;

class TilesetTerrainAction {
public:
  //! Constructor
  TilesetTerrainAction(TilesetTerrainRule *rule)
    : mRule(rule) {}

  //! Deconstructor
  virtual ~TilesetTerrainAction() = default;
  
  //! Returns the type
  virtual QString typeId() const = 0;
  
  //! Applies the terrain
  virtual void apply(TilemapTerrainLayer*, TilemapTerrainBuildContext &context, const QPoint &pos) const = 0;
  
  //! Loads the terrain
  virtual bool load(const YAML::Node &) = 0;

  //! Saves the terrain
  virtual bool save(YAML::Emitter &) const = 0;

protected:
  //! The parent rule
  TilesetTerrainRule *mRule;
};

}