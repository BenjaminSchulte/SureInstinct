#pragma once

#include <QMap>
#include "GameAsset.hpp"

namespace Aoba {
class AbstractAsset;
}

namespace Maker {
class GameProject;
class GameLinkResult;

typedef QMap<QString, AbstractGameAsset*> GameAssetsMap;

struct GameGlobalAsset {
  GameGlobalAsset() {}
  GameGlobalAsset(const QString &as, const QString &type, const QString &id, const QString &attribute)
    : mAs(as)
    , mType(type)
    , mId(id)
    , mAttribute(attribute)
  {}

  //! Returns the name
  const QString &exportName() const { return mAs; }

  //! Returns the type
  const QString &type() const { return mType; }

  //! Returns the ID
  const QString &id() const { return mId; }

  //! Returns the attribute
  const QString &attribute() const { return mAttribute; }

private:
  QString mAs;
  QString mType;
  QString mId;
  QString mAttribute;
};

class GameAssets {
public:
  //! Constructor
  GameAssets(GameProject *project) : mProject(project) {}

  //! Deconstructor
  ~GameAssets();

  //! Loads the asset types
  bool load();
  
  //! Saves the assets
  bool save();

  //! Loads the assets itself
  bool loadAssets();

  //! Builds the assets itself
  bool buildAssets();

  //! Links all assets
  bool linkAssets(GameLinkResult &result);

  //! Adds an asset
  void add(AbstractGameAsset *asset);

  //! Returns an asset set by its type
  AbstractGameAsset *getAsset(const QString &type, const QString &id) const { return mAssets[type][id]; }

  //! Prepares the asset types
  bool prepareGlobalAssets();

protected:
  //! Loads the asset types
  bool loadGlobalAssets();

  //! Links all assets
  bool linkGlobalAssets(GameLinkResult &result);

  //! The project
  GameProject *mProject;

  //! Assets per type
  QMap<QString, GameAssetsMap> mAssets;

  //! Global assets
  QVector<GameGlobalAsset> mGlobalAssets;
};

}