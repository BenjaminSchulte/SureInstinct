#pragma once

#include <QDir>
#include <QMap>
#include <QVector>
#include <QVariant>
#include "LevelConfig.hpp"

namespace Aoba {

class Level : public Asset<LevelAssetSet> {
public:
  //! Constructor
  Level(LevelAssetSet *set, const QString &id, const QDir &path);

  //! Deconstructor
  ~Level();

  //! Returns the symbol name
  QString symbolName() const;

  //! Returns the symbol name
  QString scriptNamespace() const;

  //! Returns a script name for the level
  QString levelScriptName(const QString &) const;

  //! Saves the asset
  bool save() override;

  //! Loads the asset
  bool reload() override;

  //! Builds the asset
  bool build() override;

  //! Returns the necessary FMA code
  QString getFmaCode() const override;

  //! Returns the level object files
  QStringList getFmaObjectFiles() const override;

  //! Returns all scripts
  inline const QStringList &scripts() const { return mScripts; }

  //! Returns all properties
  inline QMap<QString, QVariant> &properties() { return mProperties; }

  //! Resize the map
  bool resize(int left, int top, int right, int bottom, const QSize &newSize);

private:
  //! Converts properties to FMA code
  QString propertiesToFma(const QMap<QString, QVariant> &map) const;

  //! The containing path
  QDir mPath;

  //! List of all scripts
  QStringList mScripts;

  //! Properties
  QMap<QString, QVariant> mProperties;
};

}
