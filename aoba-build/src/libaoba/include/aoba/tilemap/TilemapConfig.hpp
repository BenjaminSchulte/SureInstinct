#pragma once

#include <QDir>
#include <QVector>
#include "../asset/Asset.hpp"
#include "../asset/LinkedAssetSet.hpp"

namespace Aoba {
class Tilemap;
class TilesetAssetSet;
class TilesetAssetSetLayer;
class TilemapAssetSet;

class TilemapAssetSetLayer {
public:
  //! Constructor
  TilemapAssetSetLayer(TilemapAssetSet *owner, const QString &id, const QString &name, TilesetAssetSetLayer *layer);

  //! Returns the ID
  inline const QString &id() const { return mId; }

  //! Returns the ID
  inline const QString &name() const { return mName; }

  //! Returns the index
  inline int index() const { return mIndex; }

  //! Returns the owner
  inline TilemapAssetSet *owner() const { return mOwner; }

  //! Returns the tileset layer
  inline TilesetAssetSetLayer *tilesetLayer() const { return mTilesetLayer; }

  //! Whether to mirror the level on X axis
  inline void setEditorMirrorX(bool value) { mEditorMirrorX = value; }

  //! Whether to mirror the level on Y axis
  inline void setEditorMirrorY(bool value) { mEditorMirrorY = value; }

  //! Whether to mirror the level on X axis
  inline bool editorMirrorX() const { return mEditorMirrorX; }

  //! Whether to mirror the level on Y axis
  inline bool editorMirrorY() const { return mEditorMirrorY; }

private:
  //! The owner
  TilemapAssetSet *mOwner;

  //! Returns the ID
  QString mId;

  //! The name
  QString mName;

  //! The tileset layer
  TilesetAssetSetLayer *mTilesetLayer;

  //! The index
  int mIndex;

  //! Whether to mirror the level on X axis
  bool mEditorMirrorX = false;

  //! Whether to mirror the level on Y axis
  bool mEditorMirrorY = false;
};

class TilemapAssetSet : public LinkedAssetSet<Tilemap> {
public:
  //! Constructor
  TilemapAssetSet(Project *project, const QString &name, TilesetAssetSet *tileset, uint16_t maxWidth, uint16_t maxHeight, const QString &compressor)
    : LinkedAssetSet<Tilemap>(name, project)
    , mName(name)
    , mTileset(tileset)
    , mMaxWidth(maxWidth)
    , mMaxHeight(maxHeight)
    , mCompressor(compressor)
  {}

  //! Returns the asset name
  inline const QString &assetName() const { return mName; }

  //! Returns the tileset
  inline TilesetAssetSet *tileset() const { return mTileset; }

  //! Returns the maximum width
  inline uint16_t maxWidth() const { return mMaxWidth; }

  //! Returns the maximum height
  inline uint16_t maxHeight() const { return mMaxHeight; }

  //! Adds a layer
  int addLayer(TilemapAssetSetLayer *layer);

  //! Returns all layers
  TilemapAssetSetLayer* layer(const QString &id) const;

  //! Returns all layers
  inline const QVector<TilemapAssetSetLayer*> layers() const { return mLayers; }

  //! Returns the compressor name
  inline const QString &compressor() const { return mCompressor; }

  //! Returns the data bank
  inline int dataBank() const { return mDataBank; }

  //! Sets the data bank
  void setDataBank(int bank) { mDataBank = bank; }

private:
  //! The name
  QString mName;

  //! The tileset
  TilesetAssetSet *mTileset;

  //! The maximum width
  uint16_t mMaxWidth;

  //! The maximum height
  uint16_t mMaxHeight;

  //! List of all layers
  QVector<TilemapAssetSetLayer*> mLayers;

  //! Compressed
  QString mCompressor;

  //! The data bank
  int mDataBank = -1;
};

}
