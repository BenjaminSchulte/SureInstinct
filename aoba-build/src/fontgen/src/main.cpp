#include <QDir>
#include <aoba/log/Log.hpp>
#include <iostream>
#include <aoba/font/PngFontGenerator.hpp>
#include <aoba/yaml/YamlTools.hpp>
#include <yaml-cpp/yaml.h>

using namespace Aoba;

QString loadLetters(const YAML::Node &node);
bool loadConfiguration(const QDir &path, PngFontGenerator &generator);

// -----------------------------------------------------------------------------
QString loadLetters(const YAML::Node &node) {
  if (node.IsSequence()) {
    QString result;
    for (const auto &item : node) {
      result += Yaml::asString(item);
    }
    return result;
  } else {
    return Yaml::asString(node);
  }
}

// -----------------------------------------------------------------------------
bool loadConfiguration(const QDir &path, PngFontGenerator &generator) {
  YAML::Node config;
  try {
    config = YAML::LoadFile(path.filePath("font.yml").toStdString().c_str());
  } catch(YAML::BadFile&) {
    Aoba::log::warn("Unable to load font.yml in given path");
    return false;
  }

  if (Yaml::asString(config["type"]) != "variable") {
    Aoba::log::warn("Generator can only generate variable width fonts");
    return false;
  }

  generator.setHiRes(Yaml::asBool(config["hires"]));
  generator.setLetters(loadLetters(config["letters"]));
  
  for (const auto &item : config["generate"]["fonts"]) {
    if (!generator.addFont(
      Yaml::asString(item["font"]),
      Yaml::asInt(item["size"]),
      Yaml::asBool(item["halfSize"])
    )) {
      return false;
    }
  }

  return true;
}

// -----------------------------------------------------------------------------
int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Missing font path" << std::endl;
    return 1;
  }

  PngFontGenerator generator;
  if (!generator.initialize()) {
    return 1;
  }

  if (!loadConfiguration(QDir(argv[1]), generator)) {
    return 1;
  }

  if (!generator.generateMissingGlyphs()) {
    return 1;
  }

  QImage image = generator.build();
  QFile outFile(QDir(argv[1]).filePath("generated.png"));
  if (!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
    Aoba::log::warn("Can not write output file");
    return 1;
  }

  image.save(&outFile, "PNG");

  return 0;
}
