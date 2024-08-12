#pragma once

#include <QDir>
#include <QSize>
#include <QVector>
#include "TilesetTileRef.hpp"
#include "TilesetConfig.hpp"
#include "../image/ImageCollection.hpp"

namespace Aoba {
class TilesetTile;
class TilesetTerrain;
class TilesetAnimationGroup;

struct TilesetBuilderAnimation {
  TilesetBuilderAnimation() : group(nullptr), firstIndex(0x7FFFFFFF) {}

  TilesetAnimationGroup *group;
  int firstIndex;
  int lastIndex = 0;
  QMap<int, int> indices;
};

class TilesetTiles {
public:
  //! Constructor
  TilesetTiles(Tileset *tileset, TilesetAssetSetTiles *config)
    : mTileset(tileset)
    , mConfig(config)
  {}

  //! Assigns bg tiles
  bool add(const QVector<TilesetBgTile> &tiles, int numX, const SnesPalettePtr &palette);

  //! Returns the tileset
  inline Tileset *tileset() const { return mTileset; }

  //! Returns the tileset
  inline TilesetAssetSetTiles *config() const { return mConfig; }

  //! Returns the BG Tiles
  inline const QVector<TilesetBgTile> &bgTiles() const { return mBgTiles; }

  //! Returns the number of bgtiles on X axis
  inline int numBgTilesX() const { return mNumBgTilesX; }

  //! Returns the number of bgtiles on X axis
  int numTilesX() const;

  //! Returns the number of bgtiles on X axis
  int numTilesY() const;

  //! Adds a bg tile mapping
  inline void addBgTileMapping(uint16_t old, const ImageCollectionPointer &generated) {
    mGeneratedBgTileMap.insert(old, generated);
  }

  //! Maps a bg tile
  inline ImageCollectionPointer mapGeneratedBgTile(uint16_t old) const {
    return mGeneratedBgTileMap[old];
  }

  //! Returns the binary name of the index
  QString imageBinaryFileName() const;

  //! Returns a binary name
  QString binaryFileName(const QString &of) const;

  //! Returns the generated palette
  inline const SnesPalettePtr &previewPalette() const { return mGeneratedPalette; }

  //! Sets the tile offset
  inline void setTileOffset(int offset) { mTileOffset = offset; }

  //! Returns the tile offset
  inline int tileOffset() const { return mTileOffset; }

private:
  //! The tileset
  Tileset *mTileset;

  //! The config
  TilesetAssetSetTiles *mConfig;

  //! List of all tiles
  QVector<TilesetBgTile> mBgTiles;

  //! Maps bg tile indices (generated on build())
  QMap<uint16_t, ImageCollectionPointer> mGeneratedBgTileMap;

  //! Number of bg tiles in the origin image
  int mNumBgTilesX = -1;

  //! Generated palette
  SnesPalettePtr mGeneratedPalette;

  //! Tile offset
  int mTileOffset = 0;
};

class TilesetLayer {
public:
  //! Constructor
  TilesetLayer(Tileset *tileset, TilesetAssetSetLayer *config)
    : mTileset(tileset)
    , mConfig(config)
  {}

  //! Deconstructor
  ~TilesetLayer();

  //! Clears the layer
  void clear();

  //! Creates a tile
  TilesetTile *createTile();

  //! Creates a tile as a copy from another one
  TilesetTile *createTile(const TilesetTile *other);

  //! Returns the tileset
  inline Tileset *tileset() const { return mTileset; }

  //! Returns the tileset
  inline TilesetAssetSetLayer *config() const { return mConfig; }

  //! Returns all tiles
  inline const QVector<TilesetTile*> allTiles() const { return mTiles; }

  //! Returns all tiles
  inline const QVector<TilesetTile*> usedTiles() const { return mUsedTiles; }

  //! Returns the binary name of the index
  QString indexBinaryFileName() const;

  //! Returns a binary name
  QString binaryFileName(const QString &of) const;

  //! Returns a tile reference
  TilesetTileRef require(int index, bool allowFlip);

private:
  //! Returns a tile reference
  TilesetTileRef requireWithoutCache(int index, bool allowFlip);

  //! The tileset
  Tileset *mTileset;

  //! The config
  TilesetAssetSetLayer *mConfig;

  //! List of all tiles
  QVector<TilesetTile*> mTiles;

  //! List of all tiles
  QVector<TilesetTile*> mUsedTiles;

  //! A quick reference cache which provides references to tile indices
  QMap<QString, TilesetTileRef> mCachedTiles;

  //! List of which tileset tiles have been used
  QMap<QString, TilesetTileRef> mUsedTilesCache;
};

struct TilesetAnimationGroupImage {
  //! Returns the tiles
  inline QVector<PaletteImagePtr> &tiles() { return mTiles; }

  //! Returns the tiles
  inline const QVector<PaletteImagePtr> &tiles() const { return mTiles; }

private:
  //! All tiles
  QVector<PaletteImagePtr> mTiles;
};

struct TilesetAnimationGroupFrame {
  enum Command {
    NO_COMMAND,
    USER_COMMAND
  };

  TilesetAnimationGroupFrame() {}

  //! Constructor
  TilesetAnimationGroupFrame(int image, uint32_t delay, Command command=NO_COMMAND, uint8_t commandParameter=0)
    : mImageIndex(image)
    , mDelay(delay)
    , mCommand(command)
    , mCommandParameter(commandParameter)
  {}

  //! Returns the image index
  inline int imageIndex() const { return mImageIndex; }

  //! Returns the delay
  inline uint32_t delay() const { return mDelay; }

  //! The command
  inline Command command() const { return mCommand; }

  //! The command parameter
  inline uint8_t commandParameter() const { return mCommandParameter; }

private:
  //! The image index
  int mImageIndex;

  //! The delay
  uint32_t mDelay;

  //! The command
  Command mCommand;

  //! The command parameter
  uint8_t mCommandParameter;
};

class TilesetAnimationGroup {
public:
  //! Constructor
  TilesetAnimationGroup(int numTiles, TilesetAssetSetTiles *tiles) : mNumTiles(numTiles), mTiles(tiles) {}

  //! Adds a new image to the list
  TilesetAnimationGroupImage &createImage() {
    mImages.push_back(TilesetAnimationGroupImage());
    return mImages[mImages.size() - 1];
  }

  //! Creates an frame
  TilesetAnimationGroupFrame &createFrame(int image, uint8_t delay, TilesetAnimationGroupFrame::Command command, uint8_t commandParameter) {
    mFrames.push_back(TilesetAnimationGroupFrame(image, delay, command, commandParameter));
    return mFrames[mFrames.size() - 1];
  }

  //! Returns an image
  inline const TilesetAnimationGroupImage &image(int index) const {
    return mImages[index];
  }

  //! Returns an frame
  inline const TilesetAnimationGroupFrame &frame(int index) const {
    return mFrames[index];
  }

  //! Returns the tiles config
  inline TilesetAssetSetTiles *tilesConfig() const { return mTiles; }

  //! Returns the number of images
  inline int numTiles() const { return mNumTiles; }

  //! Returns the number of images
  inline int numImages() const { return mImages.size(); }

  //! Returns the number of images
  inline int numFrames() const { return mFrames.size(); }

protected:
  //! The number of tiles
  int mNumTiles;

  //! The tiles config
  TilesetAssetSetTiles *mTiles;

  //! List of all images
  QVector<TilesetAnimationGroupImage> mImages;

  //! List of all frames
  QVector<TilesetAnimationGroupFrame> mFrames;
};


class Tileset : public Asset<TilesetAssetSet> {
public:
  //! Constructor
  Tileset(TilesetAssetSet *set, const QString &id, const QDir &path);

  //! Deconstructor
  ~Tileset();

  //! Returns the symbol name
  QString assetSymbolName() const;

  //! Whether this is a generated tileset
  virtual bool isGeneratedTileset() const { return false; }

  //! Returns a layer
  TilesetTiles *tiles(TilesetAssetSetTiles *config);

  //! Returns a layer
  TilesetLayer *layer(TilesetAssetSetLayer *config);

  //! Creates an animation
  TilesetAnimationGroup *createAnimationGroup(const QString &id, int numTiles, TilesetAssetSetTiles *tiles);

  //! Returns a animation
  inline TilesetAnimationGroup *animation(const QString &id) const {
    return mAnimations[id];
  }

  //! Returns a map containing all animations
  inline const QMap<QString, TilesetAnimationGroup*> &animations() const {
    return mAnimations;
  }

  //! Saves the asset
  bool save() override;

  //! Loads the asset
  bool reload() override;

  //! Builds the asset
  bool build() override;

  //! Returns additional object files to include
  QStringList getFmaObjectFiles() const override;

  //! Returns the necessary FMA code
  QString getFmaCode() const override;

  //! Returns the path
  inline const QDir &path() const { return mPath; }
  
  //! Returns the header block
  inline FMA::linker::LinkerBlock *headerBlock() const { return mHeaderBlock; }

  //! Returns the terrain, if present
  inline TilesetTerrain *terrain() const { return mTerrain; }

  //! Returns the generated animations
  inline QMap<QString, TilesetBuilderAnimation> &generatedAnimations() { return mGeneratedAnimations; }

  //! Returns the generated animations
  inline const QMap<QString, TilesetBuilderAnimation> &generatedAnimations() const { return mGeneratedAnimations; }

private:
  //! The header linker block
  FMA::linker::LinkerBlock *mHeaderBlock;

  //! The containing path
  QDir mPath;

  //! Map of all tiles
  QMap<QString, TilesetTiles*> mTiles;

  //! Map of all layers
  QMap<QString, TilesetLayer*> mLayer;

  //! Tileset animation
  QMap<QString, TilesetAnimationGroup*> mAnimations;

  //! Terrain
  TilesetTerrain *mTerrain = nullptr;

  //! List of all animation IDs
  QMap<QString, TilesetBuilderAnimation> mGeneratedAnimations;
};

class CustomTileset : public Tileset {
public:
  //! Constructor
  CustomTileset(TilesetAssetSet *set, const QString &id) : Tileset(set, id, QDir()) {
    mIsLoaded = true;
  }

  //! Whether this is a generated tileset
  bool isGeneratedTileset() const override { return true; }

  //! Loads the asset
  bool reload() override;
};

}
