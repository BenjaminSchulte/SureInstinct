#include <aoba/property/GroupPropertyType.hpp>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
Property GroupPropertyType::load(const YAML::Node &node) const {
  QMap<QString, Property> result;

  QMapIterator<QString, PropertyType*> it(mProperties);
  while (it.hasNext()) {
    it.next();

    result.insert(it.key(), it.value()->load(node[it.key().toStdString()]));
  }

  return result;
}

// -----------------------------------------------------------------------------
void GroupPropertyType::save(YAML::Emitter &yaml, const Property &value) const {
  QMap<QString, Property> map = value.toMap();
  
  yaml << YAML::BeginMap;
  QMapIterator<QString, PropertyType*> it(mProperties);
  while (it.hasNext()) {
    it.next();

    if (!map.contains(it.key())) {
      continue;
    }

    yaml << YAML::Key << it.key().toStdString() << YAML::Value;
    it.value()->save(yaml, map[it.key()]);
  }
  yaml << YAML::EndMap;
}

// -----------------------------------------------------------------------------
bool GroupPropertyType::configure(const YAML::Node &node) {
  for (const YAML::Node &child : node["properties"]) {
    PropertyType *childType = PropertyType::create(Yaml::asString(child["id"]), child);
    if (!childType) {
      return false;
    }

    addManagedProperty(Yaml::asString(child["id"]), childType);
  }

  return true;
}

// -----------------------------------------------------------------------------
void GroupPropertyType::removeAllProperties() {
  for (PropertyType *type : mProperties) {
    delete type;
  }
  mProperties.clear();
  mOrderedProperties.clear();
}

// -----------------------------------------------------------------------------
void GroupPropertyType::addManagedProperty(const QString &id, PropertyType *property) {
  if (property == nullptr) {
    return;
  }

  if (mProperties.contains(id)) {
    return;
  }

  mProperties.insert(id, property);
  mOrderedProperties.push_back(id);
}
