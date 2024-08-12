#pragma once

#include <QMap>
#include <QRect>

#include <aoba/image/ImageAccessor.hpp>
#include <aoba/palette/SnesPalette.hpp>
#include <yaml-cpp/yaml.h>

namespace Aoba {
class Sprite;
class CsvReader;

class SpriteLoader {
public:
  //! Constructor
  SpriteLoader(Sprite *sprite) : mSprite(sprite) {}

  //! Loads the Sprite
  bool load(const YAML::Node &config);

private:
  //! Loads all frames
  bool loadImages();

  //! Loads all animations
  bool loadAnimations(const YAML::Node &config);

  //! Loads all frames
  bool loadFrames(const YAML::Node &config);

  //! Loads all frames
  bool loadFrameGroups(const YAML::Node &config);

  //! Loads all frames
  bool loadFrameGroup(const QString &id, const YAML::Node &config);

  //! Loads all frames
  bool loadImageGroups(const YAML::Node &config);

  //! Loads all frames
  bool loadImageGroup(const QString &id, const YAML::Node &config);

  //! Loads a frame
  bool loadImage(CsvReader &reader);

  //! Loads a animation frame
  bool loadAnimationFrame(const QString &id, const YAML::Node &config);

  //! Loads a animation frame
  bool loadFrame(const QString &id, const YAML::Node &config);

  //! Loads an image
  QImagePtr loadImage(const QString &id);

  //! Loads an image
  SnesPalettePtr loadPalette(const QString &id);

  // The Sprite
  Sprite *mSprite;

  // List of all loaded images
  QMap<QString, QImagePtr> mImages;

  // List of all loaded palettes
  QMap<QString, SnesPalettePtr> mPalettes;

  //! The viewbox
  QRect mViewbox;
};

}
