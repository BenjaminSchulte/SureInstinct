#include <aoba/log/Log.hpp>
#include <maker/game/GameModuleHeader.hpp>
#include <yaml-cpp/yaml.h>
#include <aoba/yaml/YamlTools.hpp>

using namespace Maker;

// -----------------------------------------------------------------------------
bool GameVersionCompareString::matches(const GameVersionString &other) const {
  return other.toString() == toString();
}

// -----------------------------------------------------------------------------
bool GameModuleHeader::load() {
  Aoba::log::trace(QString("Loading module header for module: %1").arg(mPath.absolutePath()));

  YAML::Node config;
  try {
    config = YAML::LoadFile(mPath.filePath("module.yml").toStdString().c_str());
  } catch(YAML::BadFile&) {
    Aoba::log::error(QString("Unable to load module Yaml file."));
    return false;
  }

  mId = Aoba::Yaml::asString(config["id"]);
  mName = Aoba::Yaml::asString(config["name"], mId);
  mVersion = Aoba::Yaml::asString(config["version"]);

  if (mId.isEmpty()) {
    Aoba::log::error(QString("Module %1 is missing an 'id'").arg(mPath.absolutePath()));
    return false;
  }
  if (!mVersion.isValid()) {
    Aoba::log::error(QString("Module %1 is missing a valid 'version'").arg(mPath.absolutePath()));
    return false;
  }

  if (Aoba::Yaml::isArray(config["author"])) {
    for (const auto &node : config["author"]) {
      mAuthors.push_back(Aoba::Yaml::asString(node));
    }
  }
  if (Aoba::Yaml::isArray(config["includes"])) {
    for (const auto &node : config["includes"]) {
      mIncludes.push_back(Aoba::Yaml::asString(node));
    }
  }
  if (Aoba::Yaml::isObject(config["dependencies"])) {
    for (const auto &dep : config["dependencies"]) {
      mDependencies.push_back(GameModuleDependency(
        QString(dep.first.as<std::string>().c_str()),
        QString(dep.second.as<std::string>().c_str())
      ));
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
QStringList GameModuleHeader::asmIncludePath() const {
  QStringList result;
  QDir testPath(mPath.absoluteFilePath("asm"));
  if (testPath.exists()) {
    result.push_back(testPath.absolutePath());
  }
  return result;
}