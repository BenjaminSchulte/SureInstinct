#include <aoba/property/PositionPropertyType.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool PositionPropertyType::configure(const YAML::Node &node) {
  return Vector2dPropertyType::configure(node);
}
