#pragma once

#include "AssetSet.hpp"
#include "AssetLinkerObject.hpp"

namespace Aoba {

template <typename T>
class LinkedAssetSet : public AssetSet<T> {
public:
  //! Constructor
  LinkedAssetSet(const QString &id, Project *project)
    : AssetSet<T>(id, project)
    , mLinkerObject(id, project)
  {}

  //! Returns the linker object
  inline AssetLinkerObject &assetLinkerObject() { return mLinkerObject; }

  //! Links the asset set
  bool link(QStringList &objects) {
    objects = mLinkerObject.getFmaObjectFiles();
    return true;
  }

protected:
  //! The linker object
  AssetLinkerObject mLinkerObject;
};

}
