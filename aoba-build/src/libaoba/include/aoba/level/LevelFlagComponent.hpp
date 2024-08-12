#pragma once

#include <QMap>
#include <QVector>
#include <QString>
#include "./LevelComponent.hpp"

namespace Aoba {

struct LevelFlagScreenMode {
public:
  //! Constructor
  LevelFlagScreenMode() : mIndex(0), mId() {}

  //! Constructor
  LevelFlagScreenMode(int index, const QString &id) : mIndex(index), mId(id) {}

  //! The index
  inline int index() const { return mIndex; }

  //! The ID
  inline const QString &id() const { return mId; }

private:
  //! The index
  int mIndex;

  //! The ID
  QString mId;
};

struct LevelFlagInstance {
public:
  //! The screen mode
  QString screenMode;
  
  //! Whether parallax mode is enabled
  bool parallax = false;

  bool slowCameraBg1X = false;
  bool slowCameraBg1Y = false;
  bool slowCameraBg2X = false;
  bool slowCameraBg2Y = false;
  bool lockCameraBg1X = false;
  bool lockCameraBg1Y = false;
  bool lockCameraBg2X = false;
  bool lockCameraBg2Y = false;
};

class LevelFlagComponent : public LevelComponent {
public:
  //! Constructor
  LevelFlagComponent(const QString &id, const QString &defaultMode, const QVector<LevelFlagScreenMode> &modes);

  //! The type id
  static QString TypeId() { return "LevelFlagComponent"; }

  //! Returns the type ID
  QString typeId() const override { return TypeId(); }

  //! Loads the component
  bool load(Level *level, YAML::Node &config) const override;

  //! Saves the component
  bool save(Level *Level, YAML::Emitter &root) const override;
  
  //! Builds the component
  bool build(Level *level, FMA::linker::LinkerBlock *block) const override;

private:
  //! The default mode
  QString mDefaultMode;

  //! Screen modes
  QMap<QString, LevelFlagScreenMode> mModes;
};
  
}

Q_DECLARE_METATYPE(Aoba::LevelFlagInstance);
