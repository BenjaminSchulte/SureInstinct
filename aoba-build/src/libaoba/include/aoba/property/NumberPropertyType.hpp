#pragma once

#include "PropertyType.hpp"

namespace Aoba {

class NumberPropertyType : public PropertyType {
public:
  //! Constructor
  NumberPropertyType(const QString &name) : PropertyType(name) {}

  //! Returns the type ID
  static QString TypeId() { return "Aoba::NumberPropertyType"; }

  //! Returns the type ID
  QString typeId() const override { return TypeId(); };

  //! The default value
  inline void setDefaultValue(const int &value) { mDefaultValue = value; }
  
  //! Returns the default value
  inline const int &defaultValue() const { return mDefaultValue; }

  //! Configures the type
  bool configure(const YAML::Node &) override;

  //! Loads a value
  Property load(const YAML::Node &) const override;

  //! Writes a property
  void save(YAML::Emitter &, const Property &) const override;

protected:
  //! The default value
  int mDefaultValue;
};

}