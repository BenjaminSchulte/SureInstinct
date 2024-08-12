#pragma once

#include <aoba/tileset/TilesetConfig.hpp>

namespace Aoba {
class TilesetAnimationModule;

class TilesetAnimationModuleInstance : public TilesetAssetSetModuleInstance {
public:
  //! Construxtor
  TilesetAnimationModuleInstance(TilesetAssetSetModule *module, Tileset *tileset)
    : TilesetAssetSetModuleInstance(module, tileset)
  {}

  //! Deconstructor
  ~TilesetAnimationModuleInstance() {}
  
  //! Returns the type ID
  static QString TypeId() { return "Aoba::TilesetAnimationModuleInstance"; }

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

private:
  //! Builds all animation frames
  QStringList buildAnimationFrameImages(const QString &, const TilesetBuilderAnimation &animation);
};

class TilesetAnimationModule : public TilesetAssetSetModule {
public:
  //! Constructor
  TilesetAnimationModule(TilesetAssetSet *set, const QString &name)
    : TilesetAssetSetModule(set)
    , mName(name)
  {}

  //! Creates a new instance
  TilesetAssetSetModuleInstance *createInstance(Tileset *tileset) override {
    return new TilesetAnimationModuleInstance(this, tileset);
  }

  //! Returns the name
  inline const QString &name() const { return mName; }

private:
  //! The name
  QString mName;
};

}