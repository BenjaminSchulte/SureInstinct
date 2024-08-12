#pragma once

#include "AssetSet.hpp"

namespace Aoba {

class AbstractAsset {
public:
  //! Constructor
  AbstractAsset(const QString &id)
    : mIsLoaded(false)
    , mId(id)
    , mName(id)
  {}

  //! Deconstructor
  virtual ~AbstractAsset() = default;

  //! Returns the type ID
  virtual const QString &typeId() const = 0;

  //! Returns the ID
  inline const QString &id() const { return mId; }

  //! Returns the name of the asset
  const QString &name() const { return mName; }

  //! Sets the name of the asset
  virtual void setName(const QString &name) { mName = name; }

  //! Returns the binary filename
  QString binaryFileName(const QString &postfix) const;

  //! Saves the asset
  virtual bool save() {
    return false;
  }

  //! Loads the asset
  bool load() {
    if (mIsLoaded) {
      return true;
    }

    if (!reload()) {
      return false;
    }

    mIsLoaded = true;
    return true;
  }

  //! Loads the asset
  virtual bool reload() = 0;

  //! Builds the asset
  virtual bool build() = 0;

  //! Returns the necessary FMA code
  virtual QString getFmaCode() const { return ""; }

  //! Returns the project
  virtual Project *project() const = 0;

  //! Returns additional object files to include
  virtual QStringList getFmaObjectFiles() const {
    return QStringList();
  }

protected:
  //! Whether the asset is loaded
  bool mIsLoaded;

  //! The ID
  QString mId;

  //! The name
  QString mName;
};

template <typename TAssetSet>
class Asset : public AbstractAsset {
public:
  //! Constructor
  Asset(TAssetSet *set, const QString &id)
    : AbstractAsset(id)
    , mAssetSet(set)
  {
  }

  //! Deconstructor
  virtual ~Asset() = default;

  //! Returns the type ID
  const QString &typeId() const override { return mAssetSet->typeId(); }

  //! Returns the asset set
  inline TAssetSet *assetSet() const { return mAssetSet; }

  //! Returns the project
  Project *project() const override { return mAssetSet->project(); }

protected:
  //! The asset set
  TAssetSet *mAssetSet;
};

}