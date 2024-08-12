#pragma once

#include <aoba/tileset/TilesetConfig.hpp>
#include <aoba/tileset/TileFlagGroupModule.hpp>

namespace FMA {
namespace output {
class DynamicBuffer;
}
}

namespace Aoba {
class TileBitplane2dModule;
class ImageCollection;

class TileBitplane2dModuleInstance : public TilesetAssetSetModuleInstance {
public:
  //! Construxtor
  TileBitplane2dModuleInstance(TilesetAssetSetModule *module, Tileset *tileset, TilesetAssetSetModuleInstance *properties)
    : TilesetAssetSetModuleInstance(module, tileset)
    , mProperties(dynamic_cast<TileFlagGroupModuleInstance*>(properties))
  {}

  //! Deconstructor
  ~TileBitplane2dModuleInstance();
  
  //! Returns the type ID
  static QString TypeId() { return "Aoba::TileBitplane2dModuleInstance"; }

  //! Returns the type ID
  QString typeId() const override { return TypeId(); }

  //! Loads a tile
  bool loadTile(TilesetLayer*, TilesetTile*, CsvReader &) override;

  //! Hashs a tile
  QString hashTile(TilesetLayer*, const TilesetTile*) override;

  //! Generates csv header
  void writeCsvHeader(CsvWriter &) const override;

  //! Generates csv header
  void writeTile(const TilesetLayer*, const TilesetTile*, CsvWriter &) const override;

  //! Copies tiles to the new tiles
  void replaceOldTiles(const QMap<const Aoba::TilesetTile*, Aoba::TilesetTile*> &) override;

  //! Builds the instance
  bool build() override;

  //! Returns the FMA code
  QString getFmaCode() const override;

  //! Returns the collision module
  inline TileBitplane2dModule *bitplaneModule() const { return reinterpret_cast<TileBitplane2dModule*>(mModule); }

protected:
  //! Loads a layer
  bool loadLayer(TilesetLayer*);

  //! Builds a single tile
  bool buildTile(FMA::output::DynamicBuffer &buffer, const ImageAccessorPtr &image) const;

  //! Builds a row
  uint64_t buildTileRow(const ImageAccessorPtr &image, uint32_t y) const;

  //! Layers
  QMap<const TilesetLayer*, ImageCollection*> mLayers;

  //! Tiles
  QMap<const TilesetTile*, uint32_t> mTileToBitplane;

  //! Tiles
  QVector<int32_t> mBitplaneToGenerated;

  //! Properties
  TileFlagGroupModuleInstance *mProperties;
};

class TileBitplane2dModule : public TilesetAssetSetModule {
public:
  //! Constructor
  TileBitplane2dModule(TilesetAssetSet *set, const QString &name, uint32_t maxSizeInBytes, TileFlagGroupAssetSet *flagGroup)
    : TilesetAssetSetModule(set)
    , mName(name)
    , mMaxSizeInBytes(maxSizeInBytes)
    , mFlags(set, flagGroup, false)
  {}

  //! Creates a new instance
  TilesetAssetSetModuleInstance *createInstance(Tileset *tileset) override {
    return new TileBitplane2dModuleInstance(this, tileset, mFlags.group() ? mFlags.createInstance(tileset) : nullptr);
  }

  //! Returns the name
  inline const QString &name() const { return mName; }

  //! Returns the max size in bytes
  inline uint32_t maxSizeInBytes() const { return mMaxSizeInBytes; }

  //! Returns the flags
  inline const TileFlagGroupModule &flags() const { return mFlags; }

private:
  //! The name
  QString mName;

  //! Max size in bytes
  uint32_t mMaxSizeInBytes;

  //! Additional tile properties
  TileFlagGroupModule mFlags;
};

}