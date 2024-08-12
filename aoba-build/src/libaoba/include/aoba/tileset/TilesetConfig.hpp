#pragma once

#include <QSize>
#include <QDir>
#include <QVector>
#include <QSharedPointer>
#include "../asset/Asset.hpp"
#include "../asset/LinkedAssetSet.hpp"

namespace Aoba {
class CsvReader;
class CsvWriter;
class Tileset;
class TilesetTile;
class TilesetLayer;
class PaletteImage;
class PaletteGroupSet;
class TilesetAssetSet;
class TilesetAssetSetTiles;
class TilesetAssetSetLayer;

enum class TilesetLayerMode {
  // Write tiles direct into tilemap
  DIRECT,

  // Generates a separate tile index
  TILEINDEX
};

struct TilesetBgTile {
  TilesetBgTile() : valid(false) {}
  TilesetBgTile(uint8_t palette, const QSharedPointer<PaletteImage> &data, uint16_t image) : valid(true), palette(palette), image(image), data(data) {}
  bool valid;
  uint8_t palette;
  uint16_t image;
  QSharedPointer<PaletteImage> data;
};

class TilesetAssetSetTiles {
public:
  //! Constructor
  TilesetAssetSetTiles(TilesetAssetSet *owner, const QString &id, PaletteGroupSet *palette, int maxTiles);

  //! Returns the ID
  inline const QString &id() const { return mId; }

  //! Returns the index
  inline int index() const { return mIndex; }

  //! Returns the owner
  inline TilesetAssetSet *owner() const { return mOwner; }

  //! Returns the owner
  inline PaletteGroupSet *paletteGroupSet() const { return mPalette; }

  //! Returns the max tiles
  inline int maxTiles() const { return mMaxTiles; }

  //! Returns the palette base
  int paletteBase() const;

private:
  //! The owner
  TilesetAssetSet *mOwner;

  //! The palette
  PaletteGroupSet *mPalette;

  //! Returns the ID
  QString mId;
  
  //! The max amount of tiles
  int mMaxTiles;

  //! The index
  int mIndex;
};

class TilesetAssetSetLayer {
public:
  //! Constructor
  TilesetAssetSetLayer(TilesetAssetSet *owner, const QString &id, TilesetAssetSetTiles *tiles, int maxTiles, TilesetLayerMode mode, uint8_t numBackgrounds);

  //! Returns the ID
  inline const QString &id() const { return mId; }

  //! Returns the index
  inline int index() const { return mIndex; }

  //! Returns the owner
  inline TilesetAssetSet *owner() const { return mOwner; }

  //! Returns the tiles
  inline TilesetAssetSetTiles *tiles() const { return mTiles; }

  //! How many backgrounds to use for this layers
  inline uint8_t numBackgrounds() const { return mNumBackgrounds; }

  //! Returns the max tiles
  inline int maxTiles() const { return mMaxTiles; }

  //! Returns the mode
  inline TilesetLayerMode mode() const { return mMode; }

  //! Returns whether this is direct copy mode
  inline bool isDirectMode() const { return mMode == TilesetLayerMode::DIRECT; }

  //! Returns whether this is direct copy mode
  inline bool isTileIndexMode() const { return mMode == TilesetLayerMode::TILEINDEX; }

private:
  //! The owner
  TilesetAssetSet *mOwner;

  //! The mode
  TilesetLayerMode mMode;

  //! Returns the ID
  QString mId;

  //! The tiles
  TilesetAssetSetTiles *mTiles;
  
  //! The max amount of tiles
  int mMaxTiles;

  //! The index
  int mIndex;

  //! Number of backgrounds to use
  uint8_t mNumBackgrounds;
};

class TilesetAssetSetModule;

class TilesetAssetSet : public LinkedAssetSet<Tileset> {
public:
  //! Constructor
  TilesetAssetSet(Project *project, const QString &id, uint8_t headerRomBank, uint8_t animationRomBank, uint8_t tileWidth, uint8_t tileHeight, uint8_t bgTileWidth, uint8_t bgTileHeight)
    : LinkedAssetSet<Tileset>(id, project)
    , mHeaderRomBank(headerRomBank)
    , mAnimationRomBank(animationRomBank)
    , mTileWidth(tileWidth)
    , mTileHeight(tileHeight)
    , mBgTileWidth(bgTileWidth)
    , mBgTileHeight(bgTileHeight)
  {}

  //! Returns the asset type name
  inline uint8_t headerRomBank() const { return mHeaderRomBank; }

  //! Returns the asset type name
  inline uint8_t animationRomBank() const { return mAnimationRomBank; }

  //! Creates a new layer
  int addTiles(TilesetAssetSetTiles *layer);

  //! Creates a new layer
  int addLayer(TilesetAssetSetLayer *layer);

  //! Adds a module
  void addModule(TilesetAssetSetModule *mod) { mModules.push_back(mod); }

  //! Returns all modules
  inline const QVector<TilesetAssetSetModule*> &modules() const { return mModules; }

  //! Returns the bgTile width
  inline uint8_t bgTileWidth() const { return mBgTileWidth; }

  //! Returns the bgTile height
  inline uint8_t bgTileHeight() const { return mBgTileHeight; }

  //! Returns the tile width
  inline uint8_t tileWidth() const { return mTileWidth; }

  //! Returns the tile height
  inline uint8_t tileHeight() const { return mTileHeight; }

  //! Returns the tile size
  inline QSize tileSize() const { return QSize(mTileWidth, mTileHeight); }

  //! Returns the tile size
  inline QSize bgTileSize() const { return QSize(mBgTileWidth, mBgTileHeight); }

  //! Returns the number of bg tiles in X axis
  inline uint8_t numBgTilesX() const { return mTileWidth / mBgTileWidth; }

  //! Returns the number of bg tiles in Y axis
  inline uint8_t numBgTilesY() const { return mTileHeight / mBgTileHeight; }
  
  //! Returns the number of bg tiles
  inline QSize numBgTilesPerTile() const { return QSize(numBgTilesX(), numBgTilesY()); }

  //! Returns the number of bg tiles
  inline uint8_t numBgTiles() const { return numBgTilesX() * numBgTilesY(); }

  //! Returns whether the index is compressed
  inline bool indexCompressed() const { return true; }

  //! Returns all tiles
  inline const QVector<TilesetAssetSetTiles*> &tiles() const { return mTiles; }

  //! Returns all layer
  inline const QVector<TilesetAssetSetLayer*> &layer() const { return mLayers; }

  //! Returns tiles by its ID
  TilesetAssetSetTiles *getTiles(const QString &id) const {
    for (TilesetAssetSetTiles *tiles : mTiles) {
      if (tiles->id() == id) {
        return tiles;
      }
    }
    return nullptr;
  }

  //! Returns a layer
  TilesetAssetSetLayer* getLayer(const QString &id) const {
    for (TilesetAssetSetLayer *layer : mLayers) {
      if (layer->id() == id) {
        return layer;
      }
    }
    return nullptr;
  }

  //! Returns the maximum bytes
  inline int32_t maxBytesPerCompressionChunk() const { return mMaxBytesPerCompressionChunk; }

  //! Sets the maximum bytes
  void setMaxBytesPerCompressionChunk(int32_t max) { mMaxBytesPerCompressionChunk = max; }

  //! Returns the tile index multiplier
  inline uint8_t tileIndexMultiplier() const { return mTileIndexMultiplier; }

  //! Sets the tile index multiplier
  void setTileIndexMultiplier(uint8_t multiplier) { mTileIndexMultiplier = multiplier; }

private:
  //! List of all tiles
  QVector<TilesetAssetSetTiles*> mTiles;

  //! List of all layers
  QVector<TilesetAssetSetLayer*> mLayers;

  //! List of all attached sets
  QVector<TilesetAssetSetModule*> mModules;

  //! The name
  uint8_t mHeaderRomBank;

  //! The animation rom bank
  uint8_t mAnimationRomBank;

  //! The tile width
  uint8_t mTileWidth;

  //! The tile height
  uint8_t mTileHeight;

  //! The tile width
  uint8_t mBgTileWidth;

  //! The tile height
  uint8_t mBgTileHeight;
  
  //! The tile index multiplier
  uint8_t mTileIndexMultiplier = 1;

  //! The compression maximum size
  int32_t mMaxBytesPerCompressionChunk = 0x2000;
};

class TilesetAssetSetModuleInstance {
public:
  //! Constructor
  TilesetAssetSetModuleInstance(TilesetAssetSetModule *module, Tileset *tileset)
    : mModule(module)
    , mTileset(tileset)
  {}

  //! Deconstructor
  virtual ~TilesetAssetSetModuleInstance() = default;

  //! Returns an unique type ID
  virtual QString typeId() const = 0;

  //! Builds the instance
  virtual bool build() = 0;

  //! Returns the FMA code
  virtual QString getFmaCode() const {
    return "";
  }

  //! Loads a tile
  virtual bool loadTile(TilesetLayer*, TilesetTile*, CsvReader &) {
    return true;
  }

  //! Hashs a tile
  virtual QString hashTile(TilesetLayer*, const TilesetTile*) {
    return "";
  }

  //! Generates csv header
  virtual void writeCsvHeader(CsvWriter &) const {
  }

  //! Generates csv header
  virtual void writeTile(const TilesetLayer*, const TilesetTile*, CsvWriter &) const {
  }

  //! Migrates tiles
  virtual void replaceOldTiles(const QMap<const Aoba::TilesetTile*, Aoba::TilesetTile*> &) {
  }

protected:
  //! The module
  TilesetAssetSetModule *mModule;
  
  //! The tileset
  Tileset *mTileset;
};

class TilesetAssetSetModule {
public:
  //! Constructor
  TilesetAssetSetModule(TilesetAssetSet *set, bool addToSet=true) : mSet(set) {
    if (addToSet) {
      set->addModule(this);
    }
  }

  //! Deconstructor
  virtual ~TilesetAssetSetModule() {
    destroyAllInstances();
  }

  //! Returns the owner set
  inline TilesetAssetSet *set() const { return mSet; }

  //! Creates a new instance
  virtual TilesetAssetSetModuleInstance *createInstance(Tileset *) = 0;

  //! Returns the instance for a tileset
  TilesetAssetSetModuleInstance *instanceFor(Tileset *tileset) {
    if (!mInstances.contains(tileset)) {
      mInstances.insert(tileset, createInstance(tileset));
    }

    return mInstances[tileset];
  }

  //! Returns the instance for a tileset
  TilesetAssetSetModuleInstance *instanceFor(const Tileset *tileset) {
    return mInstances[tileset];
  }

protected:
  //! Destroys all instances
  void destroyAllInstances();

  //! List of all instances
  QMap<const Tileset*, TilesetAssetSetModuleInstance*> mInstances;

  //! The owner set
  TilesetAssetSet *mSet;
};

}
