#pragma once

#include <QString>
#include <yaml-cpp/yaml.h>

namespace Aoba {
namespace Yaml {

inline QString asString(const YAML::Node &node, const QString &fallback="") {
  try {
    return node.as<std::string>().c_str();
  } catch(YAML::InvalidNode&) {
    return fallback;
  } catch(YAML::BadConversion&) {
    return fallback;
  }
}

inline int asInt(const YAML::Node &node, int fallback=0) {
  try {
    return node.as<int>();
  } catch(YAML::InvalidNode&) {
    return fallback;
  } catch(YAML::BadConversion&) {
    return fallback;
  }
}

inline double asDouble(const YAML::Node &node, double fallback=0) {
  try {
    return node.as<double>();
  } catch(YAML::InvalidNode&) {
    return fallback;
  } catch(YAML::BadConversion&) {
    return fallback;
  }
}

inline int asBool(const YAML::Node &node, bool fallback=false) {
  try {
    return node.as<bool>();
  } catch(YAML::InvalidNode&) {
    return fallback;
  } catch(YAML::BadConversion&) {
    return fallback;
  }
}

inline bool isArray(const YAML::Node &node) {
  try {
    return node.IsSequence();
  } catch(YAML::InvalidNode&) {
    return false;
  }
}

inline bool isObject(const YAML::Node &node) {
  try {
    return node.IsMap();
  } catch(YAML::InvalidNode&) {
    return false;
  }
}

inline bool isString(const YAML::Node &node) {
  try {
    node.as<std::string>();
    return true;
  } catch(YAML::InvalidNode&) {
    return false;
  }
}


}
}