#pragma once

#include <QString>
#include <QVector>
#include <yaml-cpp/yaml.h>

namespace Aoba {

class TilesetTerrain;
class TilesetLayer;

class TilesetTerrainLayer {
public:
  //! Constructor
  TilesetTerrainLayer(TilesetTerrain *terrain, const QString &id, const QString &name);

  //! Returns all layers
  inline TilesetLayer* defaultLayer() const { return mDefaultLayer; }

  //! Returns the terrain
  inline TilesetTerrain *terrain() const { return mTerrain; }

  //! Returns the id
  inline const QString &id() const { return mId; }

  //! Returns the name
  inline const QString &name() const { return mName; }
  
  //! Mirror on X?
  inline bool editorMirrorX() const { return mEditorMirrorX; }

  //! Whether to mirror on X axis
  inline bool editorMirrorY() const { return mEditorMirrorY; }

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

  //! The layers
  TilesetLayer* mDefaultLayer;

  //! Whether to mirror on X axis
  bool mEditorMirrorX = false;

  //! Whether to mirror on X axis
  bool mEditorMirrorY = false;
};

}