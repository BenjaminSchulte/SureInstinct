#include <aoba/image/RawImage.hpp>
#include <aoba/image/RawImageConfig.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/asset/AssetByteListLinker.hpp>
#include <aoba/tilemap/Tilemap.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
RawImageAssetSet::RawImageAssetSet(Project *project, uint8_t headerRomBank, PaletteGroupSet *palette, const QSize &tileSize, uint8_t indexMultiplier)
  : BaseImageAssetSet("raw_image", project, headerRomBank)
  , mPalette(palette)
  , mTileSize(tileSize)
  , mIndexMultiplier(indexMultiplier)
{
}

// -----------------------------------------------------------------------------
