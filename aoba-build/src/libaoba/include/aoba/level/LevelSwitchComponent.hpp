#pragma once

#include <QVector>
#include <QMap>
#include <QVariant>
#include <QString>
#include "./LevelComponent.hpp"

namespace Aoba {

struct LevelSwitch {
  LevelSwitch() {}
  LevelSwitch(uint16_t x, uint16_t y, uint16_t tile, bool flipX, bool flipY, int8_t layer)
    : x(x), y(y), tile(tile), flipX(flipX), flipY(flipY), layer(layer) {}

  uint16_t x;
  uint16_t y;
  uint16_t tile;
  bool flipX;
  bool flipY;
  int8_t layer;
};

struct LevelSwitchInstance {
public:
  //! Switchs
  QMap<QString, QVector<LevelSwitch>> mSwitches;
};

class LevelSwitchComponent : public LevelComponent {
public:
  //! Constructor
  LevelSwitchComponent(const QString &id)
    : LevelComponent(id)
  {}

  //! The type id
  static QString TypeId() { return "LevelSwitchComponent"; }

  //! Returns the type ID
  QString typeId() const override { return TypeId(); }

  //! Loads the component
  bool load(Level *level, YAML::Node &config) const override;

  //! Saves the component
  bool save(Level *Level, YAML::Emitter &root) const override;
  
  //! Builds the component
  bool build(Level *level, FMA::linker::LinkerBlock *block) const override;

  //! Resizes content
  bool resize(Level *level, int left, int top, int right, int bottom, const QSize &newSize) const override;

  //! Returns the header symbol name
  QString headerSymbolName(Level *level) const;

  //! Returns the symbol name for a switch
  QString symbolName(Level *level, const QString &id) const;

protected:
};
  
}

Q_DECLARE_METATYPE(Aoba::LevelSwitchInstance);
