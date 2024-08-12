#pragma once

#include <yaml-cpp/yaml.h>

namespace Aoba {
class Sprite;
struct SpriteFrame;

class SpriteWriter {
public:
  //! Constructor
  SpriteWriter(Sprite *sprite) : mSprite(sprite) {}

  //! Loads the tilemap
  bool save(YAML::Emitter &);

private:
  //! writes the header
  bool writeImageGroups(YAML::Emitter &);

  //! writes the header
  bool writeFrames(YAML::Emitter &);

  //! writes the header
  bool writeFrame(YAML::Emitter &, const SpriteFrame &frame);

  //! writes the header
  bool writeFrameGroups(YAML::Emitter &);

  //! writes the header
  bool writeAnimations(YAML::Emitter &);

  //! writes the header
  bool writeAnimation(YAML::Emitter &, const SpriteAnimation &anim);

  // The tilemap
  Sprite *mSprite;
};

}