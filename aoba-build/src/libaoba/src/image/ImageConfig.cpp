#include <aoba/image/Image.hpp>
#include <aoba/image/ImageConfig.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/asset/AssetByteListLinker.hpp>
#include <aoba/tilemap/Tilemap.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
BaseImageAssetSet::BaseImageAssetSet(const char *id, Project *project, uint8_t headerRomBank)
  : LinkedAssetSet(id, project)
  , mHeaderRomBank(headerRomBank)
{
}


// -----------------------------------------------------------------------------
ImageAssetSet::ImageAssetSet(Project *project, TilemapAssetSet *tilemap, uint8_t headerRomBank)
  : BaseImageAssetSet("image", project, headerRomBank)
  , mTilemap(tilemap)
{
}

// -----------------------------------------------------------------------------
TilesetAssetSet *ImageAssetSet::tileset() const {
  return mTilemap->tileset();
}

// -----------------------------------------------------------------------------
