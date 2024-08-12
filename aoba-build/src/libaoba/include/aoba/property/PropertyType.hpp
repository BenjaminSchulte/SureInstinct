#pragma once

#include <QString>
#include <QVariant>
#include <yaml-cpp/yaml.h>

namespace Aoba {

typedef QVariant Property;

class PropertyType {
public:
  //! Constructor
  PropertyType(const QString &name) : mName(name) {}

  //! Deconstructor
  virtual ~PropertyType() = default;

  //! Returns the type ID
  virtual QString typeId() const = 0;

  //! Returns the name
  inline const QString &name() const { return mName; }

  //! Configures the type
  virtual bool configure(const YAML::Node &) = 0; 

  //! Loads a value
  virtual Property load(const YAML::Node &) const = 0;

  //! Writes a property
  virtual void save(YAML::Emitter &, const Property &) const = 0;

  //! Creates a property from a YAML node
  static PropertyType *create(const QString &, const YAML::Node &);

protected:
  //! The name
  QString mName;
};

}