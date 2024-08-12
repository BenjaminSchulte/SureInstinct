#pragma once

#include <QPointF>
#include "PropertyType.hpp"

namespace Aoba {

class Vector2dPropertyType : public PropertyType {
public:
  //! Constructor
  Vector2dPropertyType(const QString &name) : PropertyType(name) {}

  //! Returns the type ID
  static QString TypeId() { return "Aoba::Vector2dPropertyType"; }

  //! Returns the type ID
  QString typeId() const override { return TypeId(); };

  //! The default value
  inline void setDefaultValue(const QPointF &value) { mDefaultValue = value; }
  
  //! Returns the default value
  inline const QPointF &defaultValue() const { return mDefaultValue; }

  //! Configures the type
  bool configure(const YAML::Node &) override;

  //! Loads a value
  Property load(const YAML::Node &) const override;

  //! Writes a property
  void save(YAML::Emitter &, const Property &) const override;

protected:
  //! The default value
  QPointF mDefaultValue;
};

}