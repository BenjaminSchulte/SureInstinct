#include <aoba/log/Log.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/font/FontRegistry.hpp>
#include <aoba/text/TextRegistry.hpp>
#include <aoba/asset/AssetByteListLinker.hpp>

#include <aoba/compress/UncompressedWriter.hpp>
#include <aoba/compress/SimplePaletteWriter.hpp>
#include <aoba/compress/Lz77Writer.hpp>
#include <aoba/compress/BgTileWriter.hpp>
#include <aoba/compress/MapTilesWriter.hpp>
#include <aoba/compress/LargeMapTilesWriter.hpp>
#include <aoba/compress/DialogTilesWriter.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
Project::Project()
  : mFonts(new FontRegistry)
  , mTexts(new TextRegistry(this))
  , mAssetByteListLinker8(new AssetByteListLinker(1, 2))
  , mAssetByteListLinker16(new AssetByteListLinker(2, 2))
{
  registerCompressor("none", [](Project *project) -> AbstractWriter* { return new UncompressedWriter(project); });
  registerCompressor("lzw", [](Project *project) -> AbstractWriter* { return new Lz77Writer(project); });
  registerCompressor("bgtile", [](Project *project) -> AbstractWriter* { return new BgTileWriter(project); });
  registerCompressor("tilemap_tiles", [](Project *project) -> AbstractWriter* { return new MapTilesWriter(project); });
  registerCompressor("large_tilemap8", [](Project *project) -> AbstractWriter* { return new LargeMapTilesWriter(project, 1); });
  registerCompressor("dialog_tiles", [](Project *project) -> AbstractWriter* { return new DialogTilesWriter(project); });
  registerCompressor("simple_palette", [](Project *project) -> AbstractWriter* { return new SimplePaletteWriter(project); });
}

// -----------------------------------------------------------------------------
Project::~Project() {
  delete mAssetByteListLinker16;
  delete mAssetByteListLinker8;
  delete mTexts;
  delete mFonts;
}

// -----------------------------------------------------------------------------
int Project::framesPerSecond() const {
  switch (mArch) {
    case Arch::NTSC: return 60;
    case Arch::PAL: return 50;

    default:
      Aoba::log::warn("Warning: Project architecture has not been set.");
      return 60;
  }
}

// -----------------------------------------------------------------------------
bool Project::sa1enabled() const {
  if (!mProperties.contains("com.aobastudio.snes.enable_sa1")) {
    return false;
  }

  return mProperties["com.aobastudio.snes.enable_sa1"].toBool();
}

// -----------------------------------------------------------------------------
