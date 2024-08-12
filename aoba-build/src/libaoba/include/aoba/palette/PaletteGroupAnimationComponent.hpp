#pragma once

#include "./PaletteGroupComponent.hpp"

namespace Aoba {

struct PaletteGroupAnimation {
  uint8_t mPalette;
  uint8_t mEffectId;
  uint8_t mFirstColor;
  uint8_t mSize;
  uint8_t mSpeed;
};

struct PaletteGroupAnimationList {
  QVector<PaletteGroupAnimation> mAnimations;
};

class PaletteGroupAnimationComponent : public PaletteGroupComponent {
public:
  //! Constructor
  PaletteGroupAnimationComponent(const QString &id)
    : PaletteGroupComponent(id)
  {}

  //! Loads the component
  bool load(PaletteGroup *level, int index, int row, YAML::Node &config) const override;

  //! Builds the component
  bool build(PaletteGroup *level, FMA::linker::LinkerBlock *block) const override;

private:
  //! Adds an animation
  bool addAnimation(PaletteGroup *level, int index, const YAML::Node &config) const;

};
  
}

Q_DECLARE_METATYPE(Aoba::PaletteGroupAnimationList);