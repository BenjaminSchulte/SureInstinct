#pragma once

#include <QMap>
#include <aoba/tileset/TilesetConfig.hpp>
#include <aoba/tileset/TileFlagGroupConfig.hpp>

namespace Aoba {
class TileFlagGroupAssetSet;
class TileFlagGroupModule;




class TileFlagGroupModuleInstance : public TilesetAssetSetModuleInstance {
public:
  //! Construxtor
  TileFlagGroupModuleInstance(TilesetAssetSetModule *module, Tileset *tileset)
    : TilesetAssetSetModuleInstance(module, tileset)
  {}
  
  //! Returns the type ID
  static QString TypeId() { return "Aoba::TileFlagGroupModuleInstance"; }

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

  //! Builds the instance
  bool build() override;

  //! Returns the FMA code
  QString getFmaCode() const override;

  //! Migrates tiles
  void replaceOldTiles(const QMap<const Aoba::TilesetTile*, Aoba::TilesetTile*> &) override;

  //! Returns the collision module
  inline TileFlagGroupModule *groupModule() const { return reinterpret_cast<TileFlagGroupModule*>(mModule); }

  //! Calculates a single tile
  uint64_t calculateTile(const TilesetLayer *, const TilesetTile *, bool &ok) const;

protected:
  //! Collision details per tile
  QMap<const TilesetTile*, TileFlagGroupTile> mTiles;
};


class TileFlagGroupModule : public TilesetAssetSetModule {
public:
  //! Constructor
  TileFlagGroupModule(TilesetAssetSet *set, TileFlagGroupAssetSet *group, bool addToSet=true)
    : TilesetAssetSetModule(set, addToSet)
    , mGroup(group)
  {}

  //! Creates a new instance
  TilesetAssetSetModuleInstance *createInstance(Tileset *tileset) override {
    return new TileFlagGroupModuleInstance(this, tileset);
  }

  //! Returns the group
  inline TileFlagGroupAssetSet *group() const { return mGroup; }

protected:
  //! The tile flag group
  TileFlagGroupAssetSet *mGroup;
};

}