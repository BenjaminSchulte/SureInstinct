#pragma once

#include "Tileset.hpp"
#include <yaml-cpp/yaml.h>

namespace Aoba {

class TilesetTerrainLayer;
class TilesetTerrainType;
class TilesetTerrainRuleGroup;

class TilesetTerrain {
public:
  //! Constructor
  TilesetTerrain(Tileset *tileset);

  //! Deconstructor
  ~TilesetTerrain();

  //! Removes all groups and types
  void clear();

  //! Returns the tileset
  inline Tileset *tileset() const { return mTileset; }

  //! Returns a tag
  int getTag(const QString &tag);

  //! Creates a new type
  TilesetTerrainLayer *createLayer(const QString &id, const QString &name);

  //! Creates a new type
  TilesetTerrainType *createType(const QString &id, const QString &name, int icon, int index);

  //! Creates a new type
  TilesetTerrainRuleGroup *createGroup(const QString &name);

  //! Returns a layer
  inline TilesetTerrainLayer *layer(const QString &id) const { return mLayers[id]; }

  //! Returns a layer
  inline TilesetTerrainType *type(const QString &id) const { return mTypes[id]; }

  //! Returns a layer
  inline TilesetTerrainType *type(int id) const { return mTypeByIndex[id]; }

  //! Returns all types
  inline const QMap<QString, TilesetTerrainType*> &types() const { return mTypes; }

  //! Returns all layers
  inline const QMap<QString, TilesetTerrainLayer*> &layers() const { return mLayers; }

  //! Returns all groups
  inline const QVector<TilesetTerrainRuleGroup*> &groups() const { return mGroups; }

  //! Loads the terrain
  bool load(const YAML::Node &);

  //! Saves the terrain
  bool save(YAML::Emitter &) const;

private:
  //! The tileset
  Tileset *mTileset;

  //! List of all tags
  QMap<QString, int> mTags;

  //! List of all types
  QMap<QString, TilesetTerrainLayer*> mLayers;

  //! List of all types
  QMap<QString, TilesetTerrainType*> mTypes;

  //! Map of types by index
  QMap<int, TilesetTerrainType*> mTypeByIndex;

  //! List of all groups
  QVector<TilesetTerrainRuleGroup*> mGroups;
};

}