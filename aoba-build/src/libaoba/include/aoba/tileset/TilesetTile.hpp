#pragma once

#include <QVector>

namespace Aoba {
class Tileset;
class TilesetLayer;
class ImageCollection;
class TilesetTile;

class TilesetTilePart {
public:
  TilesetTilePart() : mBgTileIndex(0), mPaletteIndex(0), mAnimatedTile(false), mPriority(false), mMirrorX(false), mMirrorY(false) {}

  inline int bgTileIndex() const { return mBgTileIndex; }
  inline int paletteIndex() const { return mPaletteIndex; }
  inline bool priority() const { return mPriority; }
  inline bool mirrorX() const { return mMirrorX; }
  inline bool mirrorY() const { return mMirrorY; }
  inline bool isAnimatedTile() const { return mAnimatedTile; }
  inline const QString &animationId() const { return mAnimationId; }
  inline int animationRow() const { return mAnimationRow; }

  inline void setBgTileIndex(int value) { mBgTileIndex = value; }
  inline void setPaletteIndex(int value) { mPaletteIndex = value; }
  inline void setPriority(int value) { mPriority = value; }
  inline void setMirrorX(int value) { mMirrorX = value; }
  inline void setMirrorY(int value) { mMirrorY = value; }
  inline void setAnimation(const QString &name, int row) {
    mAnimatedTile = true;
    mAnimationId = name;
    mAnimationRow = row;
  }

  QString hash(const TilesetTile *tile, bool flipX, bool flipY) const;

protected:
  int mBgTileIndex;
  int mPaletteIndex;
  bool mAnimatedTile;
  bool mPriority;
  bool mMirrorX;
  bool mMirrorY;
  QString mAnimationId;
  int mAnimationRow;
};

class TilesetTileLayer {
public:
  TilesetTileLayer(TilesetTile *tile);
  ~TilesetTileLayer();

  void copyFrom(const TilesetTileLayer *newParent);

  inline const TilesetTilePart *parts() const { return mParts; }

  inline unsigned int numParts() const { return mNumParts; }
  TilesetTilePart &part(unsigned int index);
  TilesetTilePart &part(unsigned int x, unsigned int y);
  const TilesetTilePart &constPart(unsigned int index) const;
  const TilesetTilePart &constPart(unsigned int x, unsigned int y) const;

  QString hash(bool flipX, bool flipY) const;

protected:
  TilesetTile *mTile;
  uint8_t mNumParts;
  TilesetTilePart *mParts;
};

class TilesetTile {
public:
  TilesetTile(TilesetLayer *tileset, int originalIndex);
  ~TilesetTile();

  TilesetTile *clone(TilesetLayer *layer=nullptr, int newIndex=-1) const;

  inline int originalIndex() const { return mOriginalIndex; }
  inline TilesetLayer *layer() const { return mLayer; }

  inline uint8_t numLayers() const { return mNumLayers; }
  inline TilesetTileLayer *tileLayer(int i) const { return mLayers[i]; }
  inline const QString &editorNote() const { return mEditorNote; }
  inline void setEditorNote(const QString &note) { mEditorNote = note; }

  QString hash(bool flipX, bool flipY) const;

  int getFlipTile(bool x, bool y) const;
  void setFlipTile(bool x, bool y, int tile);

protected:
  uint8_t mNumLayers;
  int mOriginalIndex;
  TilesetLayer *mLayer;
  TilesetTileLayer **mLayers;
  QString mEditorNote;
  int mFlipTile[3];
};

} 
