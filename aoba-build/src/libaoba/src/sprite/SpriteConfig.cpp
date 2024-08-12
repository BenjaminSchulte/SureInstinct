#include <aoba/sprite/Sprite.hpp>
#include <aoba/sprite/SpriteConfig.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/asset/AssetByteListLinker.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
SpriteAssetSet::SpriteAssetSet(Project *project, const QString &typeId, PaletteGroupSet *palettes, const QString &name, uint8_t smallSize, uint8_t largeSize, uint8_t frameBank, uint8_t animationBank, uint8_t paletteOffset)
  : LinkedAssetSet<Sprite>(typeId, project)
  , mName(name)
  , mPalettes(palettes)
  , mSmallSize(smallSize)
  , mLargeSize(largeSize)
  , mFrameBank(frameBank)
  , mAnimationBank(animationBank)
  , mPaletteOffset(paletteOffset)
{
  project->lookupTable("uint16", "character_speed");

  mRecordAddresses = project->assetByteListLinker16()->createList("__asset_sprite_addresses");
  mRecordBanks = project->assetByteListLinker8()->createList("__asset_sprite_banks");
}

// -----------------------------------------------------------------------------
