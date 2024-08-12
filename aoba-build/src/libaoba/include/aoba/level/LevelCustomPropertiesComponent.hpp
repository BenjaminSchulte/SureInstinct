#pragma once

#include <QMap>
#include "./LevelComponent.hpp"
#include "../property/GroupPropertyType.hpp"

namespace Aoba {
class TextPropertyType;
class Vector2dPropertyType;
class NumberPropertyType;

class LevelCustomPropertiesComponent : public LevelComponent {
public:
  //! Constructor
  LevelCustomPropertiesComponent(const QString &id, GroupPropertyType *root)
    : LevelComponent(id)
    , mProperties(root)
  {}

  //! Deconstructor
  ~LevelCustomPropertiesComponent() {
    delete mProperties;
  }

  //! The type id
  static QString TypeId() { return "LevelCustomPropertiesComponent"; }

  //! Returns the type ID
  QString typeId() const override { return TypeId(); }

  //! Loads the component
  bool load(Level *level, YAML::Node &config) const override;

  //! Saves the component
  bool save(Level *Level, YAML::Emitter &root) const override;

  //! Resizes content
  bool resize(Level *level, int left, int top, int right, int bottom, const QSize &newSize) const override;
  
  //! Builds the component
  bool build(Level *level, FMA::linker::LinkerBlock *block) const override;

  //! Returns the properties
  inline GroupPropertyType *properties() const { return mProperties; }

private:
  //! Preloads the component
  bool preloadProperty(const QString &id, PropertyType *type, const Property &value, Level *level) const;

  //! Preloads the component
  bool preloadGroupProperty(const QString &id, GroupPropertyType *type, const Property &value, Level *level) const;

  //! Preloads the component
  bool preloadTextProperty(const QString &id, TextPropertyType *type, const Property &value, Level *level) const;

  //! Builds the component
  bool buildProperty(const QString &id, PropertyType *type, const Property &value, Level *level, FMA::linker::LinkerBlock *block) const;

  //! Builds the component
  bool buildGroupProperty(const QString &id, GroupPropertyType *type, const Property &value, Level *level, FMA::linker::LinkerBlock *block) const;

  //! Builds the component
  bool buildTextProperty(const QString &id, TextPropertyType *type, const Property &value, Level *level, FMA::linker::LinkerBlock *block) const;

  //! Builds the component
  bool buildVector2dProperty(const QString &id, Vector2dPropertyType *type, const Property &value, Level *level, FMA::linker::LinkerBlock *block) const;

  //! Builds the component
  bool buildNumberProperty(const QString &id, NumberPropertyType *type, const Property &value, Level *level, FMA::linker::LinkerBlock *block) const;

  //! Properties
  GroupPropertyType *mProperties;
};

}
