#pragma once

#include <QString>
#include <QVector>
#include <yaml-cpp/yaml.h>

namespace Aoba {

class TilesetTerrain;
class TilesetTerrainLayer;

class TilesetTerrainType {
public:
  //! Constructor
  TilesetTerrainType(TilesetTerrain *terrain, const QString &id, const QString &name, int iconTile, int index);

  //! Adds a tag
  void addTag(int tag);

  //! Adds a layer
  void addLayer(TilesetTerrainLayer *layer);

  //! Returns the index
  inline int index() const { return mIndex; }

  //! Returns all tags
  inline const QVector<int> &tags() const { return mTags; }

  //! Returns all layers
  inline const QVector<TilesetTerrainLayer*> &layers() const { return mLayers; }

  //! Returns the terrain
  inline TilesetTerrain *terrain() const { return mTerrain; }

  //! Returns the id
  inline const QString &id() const { return mId; }

  //! Returns the name
  inline const QString &name() const { return mName; }

  //! Returns the icon
  inline int iconTile() const { return mIconTile; }

  //! Loads the terrain
  bool load(const YAML::Node &);

  //! Saves the terrain
  bool save(YAML::Emitter &) const;

private:
  //! The terrain
  TilesetTerrain *mTerrain;
  
  //! The id
  QString mId;
  
  //! The name
  QString mName;

  //! The icon tile
  int mIconTile;

  //! The index
  int mIndex;

  //! The layers
  QVector<TilesetTerrainLayer*> mLayers;

  //! The tags
  QVector<int> mTags;
};

}