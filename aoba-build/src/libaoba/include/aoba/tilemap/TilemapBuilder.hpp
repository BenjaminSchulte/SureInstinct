#pragma once

namespace FMA {
namespace linker {
class LinkerBlock;
}
}

namespace Aoba {
class Tilemap;
class TilemapAssetSetLayer;

class TilemapBuilder {
public:
  //! Constructor
  TilemapBuilder(Tilemap *tilemap) : mTilemap(tilemap) {}

  //! Builds the tilemap
  bool build();

private:
  //! Builds a layer
  bool buildLayer(TilemapAssetSetLayer *layer, FMA::linker::LinkerBlock *block);

  //! Returns the layer symbol name
  QString layerSymbolName(TilemapAssetSetLayer *layer) const;

  //! The tilemap
  Tilemap *mTilemap;
};

}