#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameProject.hpp>

using namespace Maker;
//using namespace Aoba;


// -----------------------------------------------------------------------------
QStringList GameConfigNode::keys() const {
  QStringList result;
  for (const auto &value : mNode) {
    result.push_back(QString(value.first.as<std::string>().c_str()));
  }
  return result;
}

// -----------------------------------------------------------------------------
GameConfigNode GameConfigNode::operator[](const QString &id) const {
  return GameConfigNode(mProject, mNode[id.toStdString()]);
}

// -----------------------------------------------------------------------------
QVariant GameConfigNode::asVariant() const {
  if (is<bool>()) {
    return asBool();
  } else if (is<int>()) {
    return asInt();
  } else {
    return asString();
  }
}

// -----------------------------------------------------------------------------
QString GameConfigNode::asAssetFileName(const QString &type, const QString &file) const {
  if (isString()) {
    return mProject->assetFile(type + "/" + asString() + "/" + file);
  } else if (isMap()) {
    return mProject->assetFile(QString(mNode["from_file"].as<std::string>().c_str()));
  }

  return "";
}

// -----------------------------------------------------------------------------
QString GameConfigNode::asDirectAssetFileName(const QString &type, const QString &ext) const {
  if (isString()) {
    return mProject->assetFile(type + "/" + asString() + ext);
  } else if (isMap()) {
    return mProject->assetFile(QString(mNode["from_file"].as<std::string>().c_str()));
  }

  return "";
}

// -----------------------------------------------------------------------------
QString GameConfigNode::asAssetPath(const QString &type) const {
  if (isString()) {
    return mProject->assetFile(type + "/" + asString());
  }

  return "";
}

// --------------------------------------o---------------------------------------
QString GameConfigNode::asString(const QString &fallback) const {
  try {
    return QString(mNode.as<std::string>().c_str());
  } catch (YAML::InvalidNode&) {
    return fallback;
  }
}

// -----------------------------------------------------------------------------
int GameConfigNode::asInt(int fallback) const {
  try {
    return mNode.as<int>();
  } catch(YAML::InvalidNode&) {
    return fallback;
  }
}

// -----------------------------------------------------------------------------
double GameConfigNode::asDouble(double fallback) const {
  try {
    return mNode.as<double>();
  } catch(YAML::InvalidNode&) {
    return fallback;
  }
}

// -----------------------------------------------------------------------------
bool GameConfigNode::asBool(bool fallback) const {
  try {
    return mNode.as<bool>();
  } catch(YAML::InvalidNode&) {
    return fallback;
  }
}

// -----------------------------------------------------------------------------
QVector<GameConfigNode> GameConfigNode::asList() const {
  QVector<GameConfigNode> result;

  try {
    if (!mNode.IsSequence()) {
      return result;
    }
  } catch(YAML::InvalidNode&) {
    return result;
  }

  for (const auto &node : mNode) {
    result.push_back(GameConfigNode(mProject, node));
  }
  return result;
}

// -----------------------------------------------------------------------------
GameConfigReader::GameConfigReader(GameProject *project, bool valid, const YAML::Node &node)
  : GameConfigNode(project, node)
  , mValid(valid)
{
}

// -----------------------------------------------------------------------------
GameConfigReader GameConfigReader::fromFile(GameProject *project, const QString &file) {
  try {
    return GameConfigReader(project, true, YAML::LoadFile(file.toStdString().c_str()));
  } catch(YAML::BadFile&) {
    return GameConfigReader(project, false, YAML::Node());
  }
}