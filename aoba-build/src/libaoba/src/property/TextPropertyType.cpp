#include <aoba/property/TextPropertyType.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
Property TextPropertyType::load(const YAML::Node &node) const {
  return Yaml::asString(node);
}

// -----------------------------------------------------------------------------
void TextPropertyType::save(YAML::Emitter &yaml, const Property &value) const {
  yaml << value.toString().toStdString();
}

// -----------------------------------------------------------------------------
bool TextPropertyType::configure(const YAML::Node &node) {
  if (Yaml::isArray(node["fonts"])) {
    for (const auto &item : node["fonts"]) {
      mFonts.push_back(Yaml::asString(item));
    }
  }
  return true;
}
