#pragma once

#include <QImage>
#include <QSharedPointer>
#include <yaml-cpp/yaml.h>

typedef QSharedPointer<class QImage> QImagePtr;

namespace Aoba {
class Tileset;
class TilesetAnimationGroup;
class TilesetAssetSetLayer;

class TilesetWriter {
public:
  //! Constructor
  TilesetWriter(Tileset *tileset) : mTileset(tileset) {}

  //! Loads the tileset
  bool save();

private:
  //! Saves the configuration
  bool saveConfiguration();

  //! Saves the animations
  bool saveAnimations(YAML::Emitter &);

  //! Saves an animation
  bool saveAnimation(YAML::Emitter &, const QString &, TilesetAnimationGroup*);

  //! Saves a layer
  bool saveLayer(TilesetAssetSetLayer *);

  //! The tileset
  Tileset *mTileset;
};

}