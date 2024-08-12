#include <QFile>
#include <aoba/log/Log.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameBuildTarget.hpp>
#include <maker/game/GameConfigReader.hpp>

using namespace Maker;

// -----------------------------------------------------------------------------
bool GameBuildTarget::initialize() {
  if (mTargets.isEmpty()) {
    mProject->log().error("Please specify at least one build target");
    return false;
  }

  GameConfigReader config(GameConfigReader::fromFile(mProject, mProject->configFile("build_targets.yml")));
  for (const QString &target : mTargets) {
    if (!loadTarget(config, target)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool GameBuildTarget::loadTarget(const GameConfigNode &root, const QString &target) {
  GameConfigNode config(root[target]);
  if (!config.isValid()) {
    mProject->log().error("Unsupported build target: " + target);
    return false;
  }
  if (!config.isMap()) {
    return true;
  }

  if (config["extends"].isString()) {
    if (!loadTarget(root, config["extends"].asString())) {
      return false;
    }
  }

  if (config["arch"].isString()) {
    QString arch(config["arch"].asString());
    if (arch == "pal") { mProject->setArch(Aoba::Arch::PAL); }
    else if (arch == "ntsc") { mProject->setArch(Aoba::Arch::NTSC); }
    else {
      mProject->log().error("Unsupported SNES architecture: " + arch);
      return false;
    }
  }

  if (config["properties"].isMap()) {
    for (const QString &key : config["properties"].keys()) {
      mProject->properties().insert(key, config["properties"][key].asVariant());
    }
  }

  return true;
}
