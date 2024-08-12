#pragma once

#include <aoba/asset/Asset.hpp>
#include <yaml-cpp/yaml.h>
#include "GameAssetType.hpp"

namespace Maker {
class GameProject;
class GameConfigNode;
class GameLinkResult;

class AbstractGameAsset {
public:
  //! Constructor
  AbstractGameAsset(const AbstractGameAssetType *type, GameProject *project, const QString &id)
    : mProject(project)
    , mAbstractType(type)
    , mId(id)
  {}

  //! Deconstructor
  virtual ~AbstractGameAsset() = default;
  
  //! Returns the aoba asset
  virtual Aoba::AbstractAsset *aobaAsset() const = 0;

  //! Returns the ID
  inline const QString &id() const { return mId; }

  //! Returns the abstract type
  inline const AbstractGameAssetType *abstractType() const { return mAbstractType; }

  //! Converts the asset to YAML assets.yml
  virtual bool toYaml(YAML::Emitter &) const = 0;

protected:
  //! The project
  GameProject *mProject;

  //! The abstract type
  const AbstractGameAssetType *mAbstractType;

  //! The ID
  QString mId;
};

template <typename T>
class GameAsset : public AbstractGameAsset {
public:
  typedef T AobaAsset;

  //! Constructor
  GameAsset(const GameAssetType<T> *type, GameProject *project, const QString &id)
    : AbstractGameAsset(type, project, id)
  {
    mType = type;
  }

  //! Deconstructor
  virtual ~GameAsset() {
    delete mAsset;
  }

  //! Creates the asset
  inline bool create(const GameConfigNode &config) {
    mAsset = configure(config);
    if (!mAsset) {
      return false;
    }
    return true;
  }

  //! Returns the aoba asset
  Aoba::AbstractAsset *aobaAsset() const override {
    return mAsset;
  }

  //! Returns the actual asset
  inline T *asset() const { return mAsset; }
  
  //! Converts the asset to YAML assets.yml
  bool toYaml(YAML::Emitter &node) const override {
    node << mAsset->id().toStdString();
    return true;
  }

protected:
  //! Configures the asset
  virtual T* configure(const GameConfigNode &config) = 0;

  //! The type
  const GameAssetType<T> *mType;

  //! The asset
  T *mAsset = nullptr;
};

}