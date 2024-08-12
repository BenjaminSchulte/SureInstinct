#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GamePaletteGroupAssetType.hpp>
#include <maker/game/GameSpriteAssetType.hpp>
#include <aoba/palette/SnesPalette.hpp>
#include <aoba/palette/Palette.hpp>
#include <aoba/palette/PaletteGroup.hpp>
#include <aoba/image/PaletteImageAccessor.hpp>
#include <aoba/image/ImageAccessor.hpp>
#include <aoba/sprite/SpriteScriptAssetResolver.hpp>

using namespace Maker;
using namespace Aoba;

// -----------------------------------------------------------------------------
GameSpriteAsset::~GameSpriteAsset() {
  delete mPreviewImage;
}


// -----------------------------------------------------------------------------
Aoba::Sprite *GameSpriteAsset::configure(const GameConfigNode &config) {
  Aoba::SpriteAssetSet *sprites = dynamic_cast<Aoba::SpriteAssetSet*>(mType->assetSet());
  Aoba::Sprite *sprite = new Aoba::Sprite(sprites, mId, config.asAssetPath("sprites"), true);

  return sprite;
}

// -----------------------------------------------------------------------------
const QPixmap *GameSpriteAsset::previewImage() {
  if (mPreviewImage != nullptr) {
    return mPreviewImage;
  }

  const auto &frames = mAsset->animation("stand_down");
  if (frames.isEmpty()) {
    return nullptr;
  }

  const auto &frame = mAsset->frames()[frames.first().frame];
  if (frame.images.isEmpty()) {
    return nullptr;
  }

  const auto &image = mAsset->images()[frame.images.first().image];
  const auto &palette = mAsset->mainPaletteGroup()->get(image.paletteIndex)->palette()->view();
  QImagePtr data = PaletteImageAccessor(image.image->toPaletteImage()).assignPalette(palette);
  if (!data) {
    return nullptr;
  }

  mPreviewImage = new QPixmap();
  mPreviewImage->convertFromImage(*data.get());

  return mPreviewImage;
}



// -----------------------------------------------------------------------------
SpriteAssetSet *GameSpriteAssetType::configure(const GameConfigNode &config) {
  QString paletteGroupId = config["palette_group"].asString("");
  GamePaletteGroupAssetType *paletteGroups = mProject->assetTypes().get<GamePaletteGroupAssetType>(paletteGroupId);
  if (!paletteGroups) {
    mProject->log().error("Could not find palette group type " + paletteGroupId);
    return nullptr;
  }

  PaletteGroupSet *paletteGroupSet = dynamic_cast<PaletteGroupSet*>(paletteGroups->assetSet());
  SpriteAssetSet *set = new SpriteAssetSet(
    mProject->project(),
    mId,
    paletteGroupSet,
    config["id"].asString(),
    config["small"].asInt(8),
    config["large"].asInt(16),
    config["frame_bank"].asInt(0xC0),
    config["animation_bank"].asInt(0xC0),
    config["palette_offset"].asInt(0)
  );

  return set;
}

// -----------------------------------------------------------------------------
GameSpriteAsset *GameSpriteAssetType::create(const QString &id) const {
  return new GameSpriteAsset(this, mProject, id);
}

// -----------------------------------------------------------------------------
Aoba::ScriptAssetResolver *GameSpriteAssetType::createAssetResolver() const {
  return new SpriteScriptAssetResolver(dynamic_cast<SpriteAssetSet*>(mAssetSet));
}

// -----------------------------------------------------------------------------
