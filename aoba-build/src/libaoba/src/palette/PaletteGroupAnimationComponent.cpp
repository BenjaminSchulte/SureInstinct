#include <aoba/log/Log.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <aoba/palette/PaletteGroup.hpp>
#include <aoba/palette/PaletteGroupAnimationComponent.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool PaletteGroupAnimationComponent::load(PaletteGroup *group, int index, int originalIndex, YAML::Node &config) const {
  if (!config[mId.toStdString()].IsDefined()) {
    return true;
  }

  for (const auto &row : config[mId.toStdString()]) {
    if (Yaml::asInt(row["row"]) != originalIndex) {
      continue;
    }

    if (!addAnimation(group, index, row)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool PaletteGroupAnimationComponent::addAnimation(PaletteGroup *group, int index, const YAML::Node &config) const {
  PaletteGroupAnimation animation;
  QString effectName = Yaml::asString(config["effect"]);
  if (effectName == "rotate") {
    animation.mEffectId = 0;
  } else {
    Aoba::log::error("Unsupported palette animation effect: " + effectName);
    return false;
  }

  animation.mPalette = index + group->assetSet()->paletteBase();
  animation.mFirstColor = Yaml::asInt(config["first"]);
  animation.mSize = Yaml::asInt(config["count"]);
  animation.mSpeed = Yaml::asInt(config["speed"]);

  PaletteGroupAnimationList instance = group->properties()[mId].value<PaletteGroupAnimationList>();
  instance.mAnimations.push_back(animation);

  QVariant value;
  value.setValue(instance);
  group->properties()[mId] = value;

  return true;
}

// -----------------------------------------------------------------------------
bool PaletteGroupAnimationComponent::build(PaletteGroup *group, FMA::linker::LinkerBlock *block) const {
  const PaletteGroupAnimationList &instance = group->properties()[mId].value<PaletteGroupAnimationList>();

  block->writeNumber(instance.mAnimations.size(), 1);
  for (const auto &anim : instance.mAnimations) {
    block->writeNumber(anim.mEffectId | (anim.mPalette << 4), 1);
    block->writeNumber(anim.mFirstColor | (anim.mSize << 4), 1);
    block->writeNumber(anim.mSpeed, 1);
  }

  return true;
}
