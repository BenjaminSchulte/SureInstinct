#pragma once

#include <QMap>
#include <QVariant>
#include "CharacterConfig.hpp"

namespace Aoba {

class Character : public Aoba::Asset<CharacterAssetSet> {
public:
  //! Constructor
  Character(CharacterAssetSet *set, const QString &id, const QDir &path);

  //! Deconstructor
  ~Character();

  //! Returns the symbol name
  QString symbolName() const;

  //! Loads the asset
  bool reload() override;

  //! Builds the asset
  bool build() override;

  //! Returns the necessary FMA code
  QString getFmaCode() const override;

  //! Returns the level object files
  QStringList getFmaObjectFiles() const override;

  //! Returns the path
  inline const QDir &path() const { return mPath; }

  //! Returns all properties
  inline QMap<QString, QVariant> &properties() { return mProperties; }

private:
  //! Converts properties to FMA code
  QString propertiesToFma(const QMap<QString, QVariant> &map) const;

  //! The containing path
  QDir mPath;

  //! Properties
  QMap<QString, QVariant> mProperties;
};

}