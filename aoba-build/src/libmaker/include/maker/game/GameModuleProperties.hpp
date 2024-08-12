#pragma once

#include <QMap>
#include <QString>
#include <QVariant>
#include <yaml-cpp/yaml.h>

namespace Maker {

struct GameModulePropertyDefinition {
  enum Type {
    BOOLEAN,
    NUMBER,
    STRING,
    ENUM,
    INVALID
  };

  //! Constructor
  GameModulePropertyDefinition() : mType(INVALID) {}

  //! Constructor
  GameModulePropertyDefinition(const QString &name, const QString &description, const QString &property, bool defaultValue)
    : mName(name), mDescription(description), mProperty(property)
    , mType(BOOLEAN)
    , mDefaultValue(defaultValue)
  {}

  //! Constructor
  GameModulePropertyDefinition(const QString &name, const QString &description, const QString &property, int defaultValue, int minimum, int maximum)
    : mName(name), mDescription(description), mProperty(property)
    , mType(NUMBER)
    , mDefaultValue(defaultValue)
    , mMinimum(minimum)
    , mMaximum(maximum)
  {}

  //! Constructor
  GameModulePropertyDefinition(const QString &name, const QString &description, const QString &property, const QString &defaultValue, int minimum, int maximum)
    : mName(name), mDescription(description), mProperty(property)
    , mType(STRING)
    , mDefaultValue(defaultValue)
    , mMinimum(minimum)
    , mMaximum(maximum)
  {}

  //! Constructor
  GameModulePropertyDefinition(const QString &name, const QString &description, const QString &property, const QMap<QString, QString> &options, const QString &defaultValue)
    : mName(name), mDescription(description), mProperty(property)
    , mType(ENUM)
    , mValues(options)
    , mDefaultValue(defaultValue)
  {}

  //! The name
  inline const QString &name() const { return mName; }

  //! The description
  inline const QString &description() const { return mDescription; }

  //! The target property
  inline const QString &property() const { return mProperty; }

  //! Mimimum
  inline int minimum() const { return mMinimum; }

  //! Maximum
  inline int maximum() const { return mMaximum; }

  //! Options list for enum
  inline const QMap<QString, QString> &values() const { return mValues; }

  //! The type
  inline const Type &type() const { return mType; }

  //! The default value
  inline const QVariant &defaultValue() const { return mDefaultValue; }

  //! Builds the FMA code
  QString buildFmaCode(const QVariant &value) const;

private:
  //! The name
  QString mName;

  //! The description
  QString mDescription;

  //! The target property
  QString mProperty;

  //! The type
  Type mType;

  //! Options list for enum
  QMap<QString, QString> mValues;

  //! The default value
  QVariant mDefaultValue;

  //! Mimimum
  int mMinimum = -0x7FFFFFFF;

  //! Maximum
  int mMaximum = 0x7FFFFFFF;
};


class GameModuleProperties {
public:
  //! Loads properties
  bool load(const QString &, const QString &);

  //! Adds a property
  bool addProperty(const QString &id, const YAML::Node &node);

  //! Returns all properties
  inline const QMap<QString, GameModulePropertyDefinition> &properties() const { return mProperties; }

private:
  //! All properties
  QMap<QString, GameModulePropertyDefinition> mProperties;
};

}