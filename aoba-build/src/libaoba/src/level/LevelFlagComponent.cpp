#include <aoba/log/Log.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/level/Level.hpp>
#include <aoba/level/LevelConfig.hpp>
#include <aoba/level/LevelFlagComponent.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
LevelFlagComponent::LevelFlagComponent(const QString &id, const QString &defaultMode, const QVector<LevelFlagScreenMode> &modes)
  : LevelComponent(id)
  , mDefaultMode(defaultMode)
{
  for (const auto &mode : modes) {
    mModes.insert(mode.id(), mode);
  }
}

// -----------------------------------------------------------------------------
bool LevelFlagComponent::save(Level *level, YAML::Emitter &root) const {
  const LevelFlagInstance &instance = level->properties()[mId].value<LevelFlagInstance>();

  root << YAML::Key << mId.toStdString() << YAML::Value << YAML::BeginMap;
  if (instance.screenMode != mDefaultMode) {
    root << YAML::Key << "mode" << YAML::Value << instance.screenMode.toStdString();
  }
  if (instance.parallax) { root << YAML::Key << "parallax" << YAML::Value << true; }
  if (instance.slowCameraBg1X) { root << YAML::Key << "slow_camera_bg1_x" << YAML::Value << true; }
  if (instance.slowCameraBg1Y) { root << YAML::Key << "slow_camera_bg1_y" << YAML::Value << true; }
  if (instance.slowCameraBg2X) { root << YAML::Key << "slow_camera_bg2_x" << YAML::Value << true; }
  if (instance.slowCameraBg2Y) { root << YAML::Key << "slow_camera_bg2_y" << YAML::Value << true; }
  if (instance.lockCameraBg1X) { root << YAML::Key << "lock_camera_bg1_x" << YAML::Value << true; }
  if (instance.lockCameraBg1Y) { root << YAML::Key << "lock_camera_bg1_y" << YAML::Value << true; }
  if (instance.lockCameraBg2X) { root << YAML::Key << "lock_camera_bg2_x" << YAML::Value << true; }
  if (instance.lockCameraBg2Y) { root << YAML::Key << "lock_camera_bg2_y" << YAML::Value << true; }
  root << YAML::EndMap;
  return true;
}

// -----------------------------------------------------------------------------
bool LevelFlagComponent::load(Level *level, YAML::Node &_config) const {
  LevelFlagInstance instance;

  YAML::Node config = _config[mId.toStdString()];
  if (Yaml::isObject(config)) {
    instance.screenMode = Yaml::asString(config["mode"], mDefaultMode);
    instance.slowCameraBg1X = Yaml::asBool(config["slow_camera_bg1_x"], false);
    instance.slowCameraBg1Y = Yaml::asBool(config["slow_camera_bg1_y"], false);
    instance.slowCameraBg2X = Yaml::asBool(config["slow_camera_bg2_x"], false);
    instance.slowCameraBg2Y = Yaml::asBool(config["slow_camera_bg2_y"], false);
    instance.lockCameraBg1X = Yaml::asBool(config["lock_camera_bg1_x"], false);
    instance.lockCameraBg1Y = Yaml::asBool(config["lock_camera_bg1_y"], false);
    instance.lockCameraBg2X = Yaml::asBool(config["lock_camera_bg2_x"], false);
    instance.lockCameraBg2Y = Yaml::asBool(config["lock_camera_bg2_y"], false);
  } else {
    instance.screenMode = mDefaultMode;
  }

  if (!mModes.contains(instance.screenMode)) {
    Aoba::log::warn("Unsupported level screen mode: " + instance.screenMode);
    instance.screenMode = mDefaultMode;
  }

  QVariant value;
  value.setValue(instance);
  level->properties().insert(mId, value);
  return true;
}

// -----------------------------------------------------------------------------
bool LevelFlagComponent::build(Level *level, FMA::linker::LinkerBlock *block) const {
  const LevelFlagInstance &instance = level->properties()[mId].value<LevelFlagInstance>();

  block->writeNumber(0
    | mModes[instance.screenMode].index()
    , 1
  );
  block->writeNumber(0
    | (instance.slowCameraBg1X ? 0x01 : 0)
    | (instance.lockCameraBg1X ? 0x02 : 0)
    | (instance.slowCameraBg2X ? 0x04 : 0)
    | (instance.lockCameraBg2X ? 0x08 : 0)
    | (instance.slowCameraBg1Y ? 0x10 : 0)
    | (instance.lockCameraBg1Y ? 0x20 : 0)
    | (instance.slowCameraBg2Y ? 0x40 : 0)
    | (instance.lockCameraBg2Y ? 0x80 : 0)
    , 1
  );

  return true;
}
