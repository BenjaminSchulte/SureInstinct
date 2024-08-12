#pragma once

#include <QString>
#include <QVector>
#include <QMap>
#include <aoba/log/Log.hpp>
#include "GameAsset.hpp"
#include "GameAssetType.hpp"

namespace Maker {
class GameProject;

template <typename T>
using GameAssetTypeConstructor = GameAssetType<T>*(*)();

class GameAssetTypes {
public:
  //! Constructor
  GameAssetTypes(GameProject *project) : mProject(project) {}

  //! Deconstructor
  ~GameAssetTypes();

  //! Loads the asset types
  bool load();

  //! Registers default asset types
  void registerDefaultAssetTypes();

  //! Adds a new loader
  template <typename T>
  void support(const QString &id, GameAssetTypeConstructor<T> constr) {
    mConstructors.insert(id, reinterpret_cast<GameAssetTypeConstructor<Aoba::AbstractAsset> >(constr));
    mTypeOrder.push_back(id);
  }

  //! Returns all types in order
  inline const QVector<GameAssetType<Aoba::AbstractAsset>*> &assetTypesInOrder() const { return mAssetTypesOrder; }

  //! Returns a asset type
  template<typename T>
  inline T *get(const QString &id) const {
    return static_cast<T*>(reinterpret_cast<GameAssetType<typename T::AobaAsset>*>(mAssetTypes[id]));
  }

  //! Returns a asset type
  template<typename T>
  QVector<T*> getAll() const {
    QVector<T*> list;
    for (auto *item : getAll(T::TypeId())) {
      list.push_back(reinterpret_cast<T*>(item));
    }
    return list;
  }

  //! Returns all assets
  QVector<GameAssetType<Aoba::AbstractAsset>*> getAll(const QString &id) const;

  //! Returns the raw asset type record
  inline GameAssetType<Aoba::AbstractAsset>* getAbstract(const QString &id) const { 
    if (!mAssetTypes.contains(id)) { return nullptr; }
    return mAssetTypes[id];
  }

  //! Links all types
  bool linkTypes(GameLinkResult &result) const;

protected:
  //! The project
  GameProject *mProject;

  //! Supported types
  QMap<QString, GameAssetTypeConstructor<Aoba::AbstractAsset> > mConstructors;

  //! Supported tyoes in order
  QVector<QString> mTypeOrder;

  //! All types
  QMap<QString, GameAssetType<Aoba::AbstractAsset>*> mAssetTypes;

  //! All asset types in order
  QVector<GameAssetType<Aoba::AbstractAsset>*> mAssetTypesOrder;
};

}
