#include <QDebug>
#include <QRegularExpression>
#include <aoba/log/Log.hpp>
#include <maker/game/GameModuleProperties.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Maker;

// -----------------------------------------------------------------------------
QString GameModulePropertyDefinition::buildFmaCode(const QVariant &value) const {
  QRegularExpression re("^((.*)::)?([A-Za-z_0-9]+)$");
  QRegularExpressionMatch match(re.match(mProperty));

  if (!match.isValid()) {
    Aoba::log::error(QString("Invalid property name: %1").arg(mProperty));
    return "";
  }

  QString fmaValue;
  if (value.type() == QVariant::Bool) {
    fmaValue = value.toBool() ? "true" : "false";
  } else if (value.type() == QVariant::Int) {
    fmaValue = QString::number(value.toInt());
  } else {
    // TODO: better escaping!
    fmaValue = "\"" + value.toString().replace("\"", "\\\"") + "\"";
  }

  QStringList result;
  QString moduleName = match.captured(2);
  if (!moduleName.isEmpty()) { result << ("module " + moduleName); }
  result << (match.captured(3)) + "=" + fmaValue;
  if (!moduleName.isEmpty()) { result << "end"; }
  return result.join('\n');
}


// -----------------------------------------------------------------------------
bool GameModuleProperties::load(const QString &fileName, const QString &prefix) {
  YAML::Node config;
  try {
    config = YAML::LoadFile(fileName.toStdString().c_str());
  } catch(YAML::BadFile&) {
    Aoba::log::error(QString("Unable to load module YAML file: %1").arg(fileName));
    return false;
  }

  for (const auto &pair : config) {
    QString key(pair.first.as<std::string>().c_str());

    if (!addProperty(prefix + key, pair.second)) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
bool GameModuleProperties::addProperty(const QString &id, const YAML::Node &node) {
  QString name = Aoba::Yaml::asString(node["name"], id);
  QString description = Aoba::Yaml::asString(node["description"]);
  QString property = Aoba::Yaml::asString(node["property"], id);
  QString type = Aoba::Yaml::asString(node["type"]);

  if (type == "string") {
    mProperties.insert(id, GameModulePropertyDefinition(
      name, description, property,
      Aoba::Yaml::asString(node["default"]),
      Aoba::Yaml::asInt(node["min_length"], 0),
      Aoba::Yaml::asInt(node["max_length"], 0x7FFFFFFF)
    ));
  } else if (type == "boolean") {
    mProperties.insert(id, GameModulePropertyDefinition(
      name, description, property,
      Aoba::Yaml::asBool(node["default"])
    ));
  } else if (type == "enum") {
    QMap<QString, QString> options;

    for (const auto &pair : node["values"]) {
      options.insert(
        QString(pair.first.as<std::string>().c_str()),
        QString(pair.second.as<std::string>().c_str())
      );
    }

    mProperties.insert(id, GameModulePropertyDefinition(
      name, description, property,
      options,
      Aoba::Yaml::asString(node["default"])
    ));
  } else {
    Aoba::log::error(QString("Unsupported property type: %1").arg(type));
    return false;
  }

  return true;
}