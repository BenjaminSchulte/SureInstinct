#pragma once

#include <QPixmap>
#include <aoba/sprite/Sprite.hpp>
#include "GameAssetType.hpp"

namespace Maker {

class GameSpriteAsset : public GameAsset<Aoba::Sprite> {
public:
  //! Constructor
  GameSpriteAsset(const GameAssetType<Aoba::Sprite> *type, GameProject *project, const QString &id)
    : GameAsset<Aoba::Sprite>(type, project, id)
  {}

  //! Deconstructor
  ~GameSpriteAsset();

  //! Creates the asset
  Aoba::Sprite *configure(const GameConfigNode &config) override;

  //! Returns a preview image
  const QPixmap *previewImage();

private:
  //! The preview image
  QPixmap *mPreviewImage = nullptr;
};

class GameSpriteAssetType : public GameAssetType<Aoba::Sprite> {
public:
  //! Loads the sprite
  GameSpriteAsset *create(const QString &id) const override;
  
  //! Creates the asset resolver
  Aoba::ScriptAssetResolver *createAssetResolver() const override;

protected:
  //! Loads the type configuration
  Aoba::SpriteAssetSet *configure(const GameConfigNode &node) override;
  
};

}