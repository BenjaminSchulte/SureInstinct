#include <aoba/property/Vector2dPropertyType.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
Property Vector2dPropertyType::load(const YAML::Node &node) const {
  return QPointF(Yaml::asDouble(node["x"]), Yaml::asDouble(node["y"]));
}

// -----------------------------------------------------------------------------
void Vector2dPropertyType::save(YAML::Emitter &yaml, const Property &value) const {
  QPointF point(value.toPointF());

  yaml << YAML::Flow << YAML::BeginMap;
  yaml << YAML::Key << "x" << YAML::Value << point.x();
  yaml << YAML::Key << "y" << YAML::Value << point.y();
  yaml << YAML::EndMap;
}

// -----------------------------------------------------------------------------
bool Vector2dPropertyType::configure(const YAML::Node &) {
  return true;
}
