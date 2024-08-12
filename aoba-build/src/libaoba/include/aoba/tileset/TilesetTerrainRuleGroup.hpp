#pragma once

#include <QString>
#include <QPoint>
#include <QVector>
#include <yaml-cpp/yaml.h>

namespace Aoba {

class TilesetTerrain;
class TilesetTerrainRule;
class TilemapTerrainLayer;
struct TilemapTerrainBuildContext;

class TilesetTerrainRuleGroup {
public:
  // Constructor
  TilesetTerrainRuleGroup(TilesetTerrain *terrain, const QString &name);

  //! Deconstructor
  ~TilesetTerrainRuleGroup();

  //! Removes all rules
  void clear();

  //! Returns the terrain
  inline TilesetTerrain *terrain() const { return mTerrain; }

  //! Returns the name
  inline const QString &name() const { return mName; }

  //! Applies the terrain
  void apply(TilemapTerrainLayer*, TilemapTerrainBuildContext &context, const QPoint &pos) const;

  //! Creates a rule
  TilesetTerrainRule *createRule();

  //! Loads the terrain
  bool load(const YAML::Node &);

  //! Saves the terrain
  bool save(YAML::Emitter &) const;

private:
  //! The terrain
  TilesetTerrain *mTerrain;

  //! The name
  QString mName;

  //! List of all rules
  QVector<TilesetTerrainRule*> mRules;
};

}