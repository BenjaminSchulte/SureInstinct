#pragma once

#include <QPointF>
#include "Vector2dPropertyType.hpp"

namespace Aoba {

class PositionPropertyType : public Vector2dPropertyType {
public:
  //! Constructor
  PositionPropertyType(const QString &name) : Vector2dPropertyType(name) {}

  //! Returns the type ID
  static QString TypeId() { return "Aoba::PositionPropertyType"; }

  //! Returns the type ID
  QString typeId() const override { return TypeId(); };

  //! Configures the type
  bool configure(const YAML::Node &) override;
};

}