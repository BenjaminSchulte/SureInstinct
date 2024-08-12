#pragma once

#include <QMap>
#include <aoba/tileset/TilesetConfig.hpp>

namespace Rpg {

class TilesetCollisionModule;

enum class TilesetCollisionValue {
  COLLISION = 0x0010,
  LAYER0    = 0x0020,
  LAYER1    = 0x0040,

  EXIT      = 0x8000,
  EXIT_MASK = 0xFF80
};

struct TilesetTileCollision {
  enum Edge {
    NORTHWEST,
    NORTHEAST,
    SOUTHWEST,
    SOUTHEAST,
    NUM_EDGES
  };

  enum Outline {
    OUTLINE_TOP_LEFT = 0,
    OUTLINE_TOP_RIGHT = 1,
    OUTLINE_BOTTOM_LEFT = 2,
    OUTLINE_BOTTOM_RIGHT = 3,
    OUTLINE_LEFT_TOP = 4,
    OUTLINE_LEFT_BOTTOM = 5,
    OUTLINE_RIGHT_TOP = 6,
    OUTLINE_RIGHT_BOTTOM = 7,
    NUM_OUTLINES
  };

  enum EnterEffect {
    NO_ENTER_EFFECT = 0,
    MOVE_TO_DEEP = 2,
    WATER_REFLECT = 3,
    MOVE_TO_BG1 = 4,
    MOVE_TO_BG2 = 5,
    MOVE_TO_BACK = 6,
    MOVE_TO_FRONT = 7
  };

  enum MoveEffect {
    NO_MOVE_EFFECT = 0,
    SLOW = 1,
    FAST = 2,
    STAIRS_UPLEFT = 3,
    STAIRS_UPRIGHT = 4,
    STAIRS_UPDOWN = 6,
    DIAGONAL_COLLISION_SE = 8,
    DIAGONAL_COLLISION_NE = 9,
    DIAGONAL_COLLISION_SW = 10,
    DIAGONAL_COLLISION_NW = 11,
  };

  TilesetTileCollision() {
    layer[0] = 0;
    layer[1] = 0;
    collision[0] = 0;
    collision[1] = 0;
    collision[2] = 0;
    collision[3] = 0;
  }

  bool layer[2];
  bool exit = 0;
  bool collision[NUM_EDGES];

  EnterEffect onEnter = NO_ENTER_EFFECT;
  MoveEffect onMove = NO_MOVE_EFFECT;

  TilesetTileCollision flipped(bool x, bool y) const;

  inline bool hasDiagonalCollision() const;

  inline int layerNumber() const { return (layer[0] ? 1 : 0) + (layer[1] ? 2 : 0); }

  inline bool isFreeLayer() const { return !layer[0] && !layer[1]; }

  inline bool isSingleLayer() const { return layer[0] != layer[1]; }

  inline bool layerCollidesWith(const TilesetTileCollision &other) const {
    return isSingleLayer() && other.isSingleLayer() && layer[0] != other.layer[0];
  }

  static inline Edge mirrorEdgeX(Edge edge) {
    return (Edge)(edge ^ 1);
  }

  static inline Edge mirrorEdgeY(Edge edge) {
    return (Edge)(edge ^ 2);
  }

  void setCollision(int index, bool value) {
    collision[index] = value;
    if (hasCollision()) { onMove = NO_MOVE_EFFECT; }
  }

  void setEvent(EnterEffect enter) {
    onEnter = enter;
  }

  void setMove(MoveEffect move) {
    onMove = move;
    if (move != NO_MOVE_EFFECT) {
      collision[0] = false;
      collision[1] = false;
      collision[2] = false;
      collision[3] = false;
    }
  }

  inline bool hasCollision() const {
    return collision[0] || collision[1] || collision[2] || collision[3];
  }

  static Outline oppositeOutline(Outline outline);

  bool collidesAt(Outline outline) const;

  bool endsCollisionAt(Outline outline, const TilesetTileCollision &other) const;
};



class TilesetCollisionModuleInstance : public Aoba::TilesetAssetSetModuleInstance {
public:
  //! Construxtor
  TilesetCollisionModuleInstance(Aoba::TilesetAssetSetModule *module, Aoba::Tileset *tileset)
    : Aoba::TilesetAssetSetModuleInstance(module, tileset)
  {}
  
  //! Returns the type ID
  static QString TypeId() { return "Rpg::TilesetCollisionModuleInstance"; }

  //! Returns the type ID
  QString typeId() const override { return TypeId(); }

  //! Loads a tile
  bool loadTile(Aoba::TilesetLayer*, Aoba::TilesetTile*, Aoba::CsvReader &) override;

  //! Hashs a tile
  QString hashTile(Aoba::TilesetLayer*, const Aoba::TilesetTile*) override;

  //! Generates csv header
  void writeCsvHeader(Aoba::CsvWriter &) const override;

  //! Generates csv header
  void writeTile(const Aoba::TilesetLayer*, const Aoba::TilesetTile*, Aoba::CsvWriter &) const override;

  //! Builds the instance
  bool build() override;

  //! Returns the FMA code
  QString getFmaCode() const override;

  //! Clears all items
  inline void clear() {
    mCollision.clear();
  }

  //! Adds a tile
  inline void addTile(const Aoba::TilesetTile *tile, const TilesetTileCollision &col) {
    mCollision[tile] = col;
  }

  //! Returns the collision module
  inline TilesetCollisionModule *collisionModule() const { return reinterpret_cast<TilesetCollisionModule*>(mModule); }

  //! Returns the info for a specific record
  inline TilesetTileCollision forTile(const Aoba::TilesetTile *tile) const { return mCollision[tile]; }
 
protected:
  //! Collision details per tile
  QMap<const Aoba::TilesetTile*, TilesetTileCollision> mCollision;
};



class TilesetCollisionModule : public Aoba::TilesetAssetSetModule {
public:
  //! Constructor
  TilesetCollisionModule(Aoba::TilesetAssetSet *set, const QString &name)
    : Aoba::TilesetAssetSetModule(set)
    , mName(name)
  {}

  //! Creates a new instance
  Aoba::TilesetAssetSetModuleInstance *createInstance(Aoba::Tileset *tileset) override {
    return new TilesetCollisionModuleInstance(this, tileset);
  }

  //! Returns the asset name
  inline const QString &assetName() const { return mName; }

  //! Adds an event
  void addEvent(const QString &event, int id) { mEvents.insert(event, id); }

  //! Adds an movement
  void addMovement(const QString &move, int id) { mMovements.insert(move, id); }

  //! Returns a collision as integer
  uint16_t toInt(const TilesetTileCollision &c) const;

  //! Returns all events
  inline const QMap<QString, int> &events() const { return mEvents; }

  //! Returns all events
  inline const QMap<QString, int> &movements() const { return mMovements; }

  //! Returns an event from a string
  QString eventToString(int event) const;

  //! Returns an event from a string
  QString movementToString(int event) const;

protected:
  //! List of all events
  QMap<QString, int> mEvents;

  //! List of all movements
  QMap<QString, int> mMovements;

  //! The name
  QString mName;
};

}