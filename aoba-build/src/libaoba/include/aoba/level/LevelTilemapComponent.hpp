#pragma once

#include <QVector>
#include <QVariant>
#include "./LevelComponent.hpp"

namespace Aoba {
class TilemapAssetSetLayer;
class TilemapAssetSet;
class Tilemap;

struct LevelTilemapRenderOrder {
  TilemapAssetSetLayer *layer;
  QString name;
  bool renderPriority = true;
  bool renderNonPriority = true;
  bool main = true;
  bool sub = true;
};

struct LevelTilemap {
  LevelTilemap() {}
  LevelTilemap(Tilemap *tilemap) : tilemap(tilemap) {}
  Tilemap *tilemap;
};

class LevelTilemapComponent : public LevelComponent {
public:
  //! Constructor
  LevelTilemapComponent(const QString &id, const TilemapAssetSet *tilemaps, const QVector<LevelTilemapRenderOrder> &order)
    : LevelComponent(id)
    , mTilemaps(tilemaps)
    , mRenderOrder(order)
  {}

  //! The type id
  static QString TypeId() { return "LevelTilemapComponent"; }

  //! Returns the type ID
  QString typeId() const override { return TypeId(); }

  //! Loads the component
  bool load(Level *level, YAML::Node &config) const override;

  //! Saves the component
  bool save(Level *Level, YAML::Emitter &root) const override;
  
  //! Builds the component
  bool build(Level *level, FMA::linker::LinkerBlock *block) const override;

  //! Resizes content
  bool resize(Level *level, int left, int top, int right, int bottom, const QSize &newSize) const override;

  //! Returns the levels tilemap
  Tilemap *tilemap(Level *level) const;

  //! Returns the levels tilemap
  void setTilemap(Level *level, Tilemap *) const;

  //! Returns the render order
  inline QVector<LevelTilemapRenderOrder> renderOrder() const { return mRenderOrder; }

  //! Returns the tilemap type
  inline const TilemapAssetSet *tilemaps() const { return mTilemaps; }

private:
  //! Tilemaps
  const TilemapAssetSet *mTilemaps;

  //! The render order
  QVector<LevelTilemapRenderOrder> mRenderOrder;
};
  
}

Q_DECLARE_METATYPE(Aoba::LevelTilemap);
