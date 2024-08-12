#pragma once

#include <QVector>
#include <QMap>
#include "PropertyType.hpp"

namespace Aoba {

class GroupPropertyType : public PropertyType {
public:
  //! Constructor
  GroupPropertyType(const QString &name) : PropertyType(name) {}

  //! Deconstructor
  ~GroupPropertyType() {
    removeAllProperties();
  }

  //! Returns the type ID
  static QString TypeId() { return "Aoba::GroupPropertyType"; }

  //! Returns the type ID
  QString typeId() const override { return TypeId(); };

  //! Removes all properties
  void removeAllProperties();

  //! Adds a property
  void addManagedProperty(const QString &id, PropertyType *property);

  //! Configures the type
  bool configure(const YAML::Node &) override;

  //! Loads a value
  Property load(const YAML::Node &) const override;

  //! Writes a property
  void save(YAML::Emitter &, const Property &) const override;

  //! Returns a property
  inline PropertyType *get(const QString &id) const { return mProperties[id]; }

  //! Returns all properties in order
  inline const QVector<QString> &keysInOrder() const { return mOrderedProperties; }

protected:
  //! Map with all properties
  QMap<QString, PropertyType*> mProperties;

  //! Ordered list of all properties
  QVector<QString> mOrderedProperties;
};

}