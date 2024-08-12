#pragma once

class QByteArray;
class QBuffer;

namespace FMA {
namespace linker {
class LinkerBlock;
}
}

namespace Aoba {
class TilemapLayer;

class TilemapLayerBuilder {
public:
  //! Constructor
  TilemapLayerBuilder(TilemapLayer *layer) : mLayer(layer) {}

  //! Builds the layer
  bool build(FMA::linker::LinkerBlock *);

private:
  //! Generates the byte array
  QByteArray generateMapData();

  //! Generates the byte array
  void generateTileIndexMapData(QBuffer &);

  //! Generates the byte array
  void generateRawMapData(QBuffer &);

  //! The layer
  TilemapLayer *mLayer;
};

}