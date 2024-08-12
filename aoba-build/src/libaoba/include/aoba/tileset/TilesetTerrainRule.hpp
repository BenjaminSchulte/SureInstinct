#pragma once

#include <QVector>
#include <yaml-cpp/yaml.h>

namespace Aoba {

class TilesetTerrainRuleGroup;
class TilesetTerrainTest;
class TilesetTerrainAction;
class TilemapTerrainLayer;
struct TilemapTerrainBuildContext;

class TilesetTerrainRule {
public:
  //! Constructor
  TilesetTerrainRule(TilesetTerrainRuleGroup *group);

  //! Deconstructor
  ~TilesetTerrainRule();

  //! Removes all tests and processors
  void clear();

  //! Applies the terrain
  void apply(TilemapTerrainLayer*, TilemapTerrainBuildContext &context, const QPoint &pos) const;

  //! Creates an action by its type ID
  static TilesetTerrainAction *createActionById(TilesetTerrainRule *parent, const QString &id);

  //! Creates a condition by its type ID
  static TilesetTerrainTest *createConditionById(TilesetTerrainRule *parent, const QString &id);

  //! Returns the parent group
  inline TilesetTerrainRuleGroup *group() const { return mGroup; }

  //! Returns all conditions
  inline const QVector<TilesetTerrainTest*> &conditions() const { return mConditions; }

  //! Returns all conditions
  inline const QVector<TilesetTerrainAction*> &actions() const { return mActions; }

  //! Whether this can be flipped
  inline bool canFlipX() const { return mCanFlipX; }

  //! Whether this can be flipped
  inline bool canFlipY() const { return mCanFlipY; }

  //! Loads the terrain
  bool load(const YAML::Node &);

  //! Saves the terrain
  bool save(YAML::Emitter &) const;

protected:
  //! The group
  TilesetTerrainRuleGroup *mGroup;

  //! List of all conditions
  QVector<TilesetTerrainTest*> mConditions;

  //! List of all conditions
  QVector<TilesetTerrainAction*> mActions;

  //! Whether this can be flipped on X axis
  bool mCanFlipX = false;

  //! Whether this can be flipped on Y axis
  bool mCanFlipY = false;
};

}
