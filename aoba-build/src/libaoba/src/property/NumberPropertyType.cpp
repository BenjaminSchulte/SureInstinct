#include <aoba/property/NumberPropertyType.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
Property NumberPropertyType::load(const YAML::Node &node) const {
  return Yaml::asInt(node);
}

// -----------------------------------------------------------------------------
void NumberPropertyType::save(YAML::Emitter &yaml, const Property &value) const {
  yaml << value.toInt();
}

// -----------------------------------------------------------------------------
bool NumberPropertyType::configure(const YAML::Node &) {
  return true;
}
