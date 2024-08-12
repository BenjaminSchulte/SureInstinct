#include <aoba/property/PropertyType.hpp>
#include <aoba/property/GroupPropertyType.hpp>
#include <aoba/property/TextPropertyType.hpp>
#include <aoba/property/PositionPropertyType.hpp>
#include <aoba/property/Vector2dPropertyType.hpp>
#include <aoba/property/NumberPropertyType.hpp>
#include <aoba/log/Log.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
PropertyType *PropertyType::create(const QString &id, const YAML::Node &node) {
  QString typeId = Yaml::asString(node["type"]);
  PropertyType *item ;

  if (typeId == "text") { item = new TextPropertyType(id); }
  else if (typeId == "position") { item = new PositionPropertyType(id); }
  else if (typeId == "vector2d") { item = new Vector2dPropertyType(id); }
  else if (typeId == "number") { item = new NumberPropertyType(id); }
  else {
    Aoba::log::error("Unsupported property type: " + typeId);
    return nullptr;
  }

  if (!item->configure(node)) {
    delete item;
    return nullptr;
  }

  return item;
}