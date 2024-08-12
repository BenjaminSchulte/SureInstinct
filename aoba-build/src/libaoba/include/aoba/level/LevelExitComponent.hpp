#pragma once

#include <QVector>
#include <QString>
#include "./LevelComponent.hpp"

namespace Aoba {

struct LevelExit {
  LevelExit() {}
  LevelExit(uint16_t x, uint16_t y, const QString &script) : x(x), y(y), script(script) {}

  uint16_t x;
  uint16_t y;
  QString script;
};

struct LevelExitInstance {
public:
  //! Exits
  QVector<LevelExit> mExits;
};

class LevelExitComponent : public LevelComponent {
public:
  //! Constructor
  LevelExitComponent(const QString &id, uint8_t bank)
    : LevelComponent(id)
    , mBank(bank)
  {}

  //! The type id
  static QString TypeId() { return "LevelExitComponent"; }

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

protected:
  //! The bank to store the exits to
  uint8_t mBank;
};
  
}

Q_DECLARE_METATYPE(Aoba::LevelExitInstance);
