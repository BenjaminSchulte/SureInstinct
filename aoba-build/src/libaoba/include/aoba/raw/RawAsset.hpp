#pragma once

#include <QString>
#include <QSharedPointer>
#include "../asset/Asset.hpp"
#include "../asset/LinkedAssetSet.hpp"

namespace Aoba {
class RawAsset;

class RawAssetSet : public LinkedAssetSet<RawAsset> {
public:
  //! Constructor
  RawAssetSet(Project *project, int bank=-1);

  //! Deconstructor
  ~RawAssetSet() {}

  //! Returns the bank
  inline int bank() const { return mBank; }

private:
  //! The bank
  int mBank;
};

class RawAsset : public Asset<RawAssetSet> {
public:
  //! Constructor
  RawAsset(RawAssetSet *set, const QString &id, const QString &file);

  //! Deconstructor
  ~RawAsset() { }

  //! Returns the sumbol name
  QString symbolName() const;

  //! Loads the asset
  bool reload() override { return true; }

  //! Builds the asset
  bool build() override;

  //! REturns the FMA objects
  QStringList getFmaObjectFiles() const override;

  //! Returns the necessary FMA code
  QString getFmaCode() const override { return ""; }

  //! Returns the file
  inline const QString &file() const { return mFile; }

protected:
  //! The containing file
  QString mFile;
};

}