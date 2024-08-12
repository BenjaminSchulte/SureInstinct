#pragma once

#include <iostream>
#include <QStringList>
#include <aoba/log/Log.hpp>
#include <QVector>

namespace Aoba {
class Project;

class AbstractAssetSet {
public:
  //! Constructor
  AbstractAssetSet(const QString &id, Project *project)
    : mTypeId(id)
    , mProject(project)
  {}

  //! Deconsturctor
  virtual ~AbstractAssetSet() = default;

  //! Returns the type ID
  inline const QString &typeId() const { return mTypeId; }

  //! Returns the project
  inline Project *project() const { return mProject; }

  //! Links the asset set
  virtual bool link(QStringList &) { return true; }

protected:
  //! The type ID
  QString mTypeId;

  //! The project
  Project *mProject;
};

template <typename T>
class AssetSet : public AbstractAssetSet {
public:
  //! Constructor
  AssetSet(const QString &id, Project *project) : AbstractAssetSet(id, project) {}

  //! Adds an item to the set
  virtual void add(T* item) {
    mItems.push_back(item);
  }

  //! Returns an item by its ID
  T *get(int id) const {
    return mItems[id - firstAssetId()];
  }

  //! Returns an item by its ID
  T *get(const QString &id) const {
    for (T* item : mItems) {
      if (item->id() == id) {
        return item;
      }
    }

    return nullptr;
  }

  //! Returns an item by its ID
  virtual int32_t assetId(const QString &id) const {
    int32_t index = firstAssetId();
    for (T* item : mItems) {
      if (item->id() == id) {
        return index;
      }

      index += 1;
    }

    return -1;
  }

  //! Returns the first asset ID
  virtual int32_t firstAssetId() const { return 0; }

  //! Builds all assets
  bool loadAll() const {
    for (T* item : mItems) {
      std::cout << "Loading \x1b[34;1m" << mTypeId.toStdString() << "\x1b[32;1m " << item->id().toStdString().c_str() << "\x1b[m" << std::endl;
      if (!item->load()) {
        return false;
      }
    }

    return true;
  }

  //! Builds all assets
  bool buildAll() const {
    for (T* item : mItems) {
      std::cout << "Building \x1b[34;1m" << mTypeId.toStdString() << "\x1b[32;1m " << item->id().toStdString().c_str() << "\x1b[m" << std::endl;
      if (!item->build()) {
        return false;
      }
    }

    return true;
  }

  //! Returns the FMA asset code
  QString getFmaCode() const {
    QStringList list;
    for (T* item : mItems) {
      list << item->getFmaCode();
    }
    return list.join('\n');
  }

  //! Returns additional object files to include
  virtual QStringList getFmaObjectFiles() const {
    QStringList list;
    for (T* item : mItems) {
      list += item->getFmaObjectFiles();
    }
    return list;
  }

  //! Returns the count
  inline int count() const { return mItems.count(); }
  
  //! Returns all items
  inline const QVector<T*> &all() const { return mItems; }

protected:
  //! List of all items
  QVector<T*> mItems;
};

}