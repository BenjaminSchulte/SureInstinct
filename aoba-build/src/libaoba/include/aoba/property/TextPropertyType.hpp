#pragma once

#include "PropertyType.hpp"

namespace Aoba {

class TextPropertyType : public PropertyType {
public:
  //! Constructor
  TextPropertyType(const QString &name) : PropertyType(name) {}

  //! Returns the type ID
  static QString TypeId() { return "Aoba::TextPropertyType"; }

  //! Returns the type ID
  QString typeId() const override { return TypeId(); };

  //! The default value
  inline void setDefaultValue(const QString &value) { mDefaultValue = value; }
  
  //! Returns the default value
  inline const QString &defaultValue() const { return mDefaultValue; }

  //! Translatable
  inline void setTranslatable(bool value) { mTranslatable = value; }

  //! Translatable
  inline bool translatable() const { return mTranslatable; }

  //! Configures the type
  bool configure(const YAML::Node &) override; 

  //! Loads a value
  Property load(const YAML::Node &) const override;

  //! Writes a property
  void save(YAML::Emitter &, const Property &) const override;

  //! Returns all fonts
  inline const QStringList &fonts() const { return mFonts; }

protected:
  //! Whether the property is translatable
  bool mTranslatable;

  //! The default value
  QString mDefaultValue;

  //! List of all fonts to compile for
  QStringList mFonts;
};

}