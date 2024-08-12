#pragma once

#include <QMap>
#include <QString>
#include <aoba/tileset/TilesetConfig.hpp>
#include <aoba/tileset/TilesetTileRef.hpp>

namespace Aoba {
class TilesetTileListModule;

typedef QVector<TilesetTileRef> TilesetTileRefList;

class TilesetTileListModuleInstance : public TilesetAssetSetModuleInstance {
public:
  //! Construxtor
  TilesetTileListModuleInstance(TilesetAssetSetModule *module, Tileset *tileset)
    : TilesetAssetSetModuleInstance(module, tileset)
  {}
  
  //! Returns the type ID
  static QString TypeId() { return "Aoba::TilesetTileListModuleInstance"; }

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
  inline TilesetTileListModule *listModule() const { return reinterpret_cast<TilesetTileListModule*>(mModule); }

protected:
  QMap<const TilesetLayer *, TilesetTileRefList> mTiles;
};


class TilesetTileListModule : public TilesetAssetSetModule {
public:
  //! Constructor
  TilesetTileListModule(TilesetAssetSet *set, const QString &name, uint8_t bytesPerTile, uint8_t listRomBank)
    : TilesetAssetSetModule(set)
    , mName(name)
    , mListRomBank(listRomBank)
    , mBytesPerTile(bytesPerTile)
  {}

  //! Creates a new instance
  TilesetAssetSetModuleInstance *createInstance(Tileset *tileset) override {
    return new TilesetTileListModuleInstance(this, tileset);
  }

  //! Returns the name
  inline const QString &name() const { return mName; }

  //! Returns the bytes per tile
  inline uint8_t bytesPerTile() const { return mBytesPerTile; }

  //! Returns the bytes per tile
  inline uint8_t listRomBank() const { return mListRomBank; }

private:
  //! The name
  QString mName;

  //! The bytes per tile
  uint8_t mListRomBank;

  //! The bytes per tile
  uint8_t mBytesPerTile;

};

}