#pragma once

#include <QString>
#include <aoba/asset/Asset.hpp>
#include <aoba/asset/AssetSet.hpp>
#include <aoba/script/ScriptAssetResolver.hpp>
#include <maker/game/GameLinkResult.hpp>

namespace Maker {
class GameConfigNode;
class GameProject;
class AbstractGameAsset;

class AbstractGameAssetType {
public:
  //! Deconstructor
  virtual ~AbstractGameAssetType() = default;

  //! Returns the ID
  virtual const QString &id() const = 0;

  //! Returns the type ID
  virtual QString typeId() const = 0;

  //! Loads an asset
  virtual bool load(Aoba::AbstractAsset *asset) const;

  //! Builds an asset
  virtual bool build(Aoba::AbstractAsset *asset) const;

  //! Links an asset
  virtual bool link(Aoba::AbstractAsset *asset, GameLinkResult &result) const;

  //! Links an asset
  virtual bool linkType(GameLinkResult &) const { return true; }
};

template <typename T>
class GameAssetType : public AbstractGameAssetType {
public:
  //! Defindes the aoba asset
  typedef T AobaAsset;

  //! Deconstuctor
  virtual ~GameAssetType() {
    delete mAssetSet;
    delete mAssetResolver;
  }

  //! Sets up the type
  inline bool setUp(const QString &id, GameProject *project, const GameConfigNode &node) {
    mId = id;
    mProject = project;

    mAssetSet = configure(node);
    if (!mAssetSet) {
      return false;
    }

    mAssetResolver = createAssetResolver();

    return true;
  }

  //! Returns the TypeID
  static QString TypeId() { return typeid(T).name(); }

  //! Returns the typeID
  QString typeId() const override { return TypeId(); }

  //! Returns the dependencies from the own resource type
  virtual QStringList dependencies(const GameConfigNode &) const { return {}; }

  //! Returns the ID
  const QString &id() const override { return mId; }

  //! Loads the level
  virtual AbstractGameAsset *create(const QString &id) const = 0;

  //! Returns the asset set
  inline Aoba::AssetSet<T> *assetSet() const { return mAssetSet; }

  //! Returns the asset resolver
  inline Aoba::ScriptAssetResolver *assetResolver() const { return mAssetResolver; }

  //! Links an asset
  bool linkType(GameLinkResult &result) const override {
    QStringList objects;
    if (!mAssetSet->link(objects)) {
      return false;
    }
    for (const QString &obj : objects) {
      result.addObject(obj);
    }
    return true;
  }


protected:
  //! Loads the type configuration
  virtual Aoba::AssetSet<T> *configure(const GameConfigNode &node) = 0;

  //! Loads an asset
  virtual Aoba::ScriptAssetResolver *createAssetResolver() const {
    return nullptr;
  }

  //! The asset ID
  QString mId;

  //! The game project
  GameProject *mProject = nullptr;

  //! The asset set
  Aoba::AssetSet<T> *mAssetSet = nullptr;

  //! The asset resolver
  Aoba::ScriptAssetResolver *mAssetResolver = nullptr;
};

}