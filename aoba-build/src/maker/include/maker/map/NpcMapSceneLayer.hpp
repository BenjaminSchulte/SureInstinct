#pragma once 

#include <aoba/level/LevelNpcComponent.hpp>
#include <QGraphicsItem>
#include "MapSceneObjectLayer.hpp"

namespace Maker {
class GameProject;
class GameCharacterAsset;
}

class GraphicsNpcItem : public QGraphicsItem {
public:
  //! Constructor
  GraphicsNpcItem(Maker::GameProject *project, Aoba::Level*, Aoba::LevelNpcComponent *mod, uint32_t npcId, QGraphicsItem *parent = nullptr);

  //! Returns the level
  inline Aoba::Level *level() const { return mLevel; }

  //! Returns the module
  inline Aoba::LevelNpcComponent *npcModule() const { return mNpcModule; }

  //! Returns the NPC id
  inline uint32_t npcId() const { return mNpcId; }

  //! Returns the bounding rect
  QRectF boundingRect() const override;

  //! Paints the NPC
  void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

protected:
  //! Finds the NPC
  bool findNpc(const Aoba::LevelNpc **npc, const Aoba::LevelNpcInstance **instance) const;

  //! Change value
  QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

  //! The project
  Maker::GameProject *mProject;

  //! The level
  Aoba::Level *mLevel;

  //! NPC module
  Aoba::LevelNpcComponent *mNpcModule;

  //! The NPC ID
  uint32_t mNpcId;

  //! The cached character
  Maker::GameCharacterAsset *mCachedCharacter = nullptr;

  //! The character pixmap
  QPixmap mPixmap;
};


class NpcMapSceneLayer : public MapSceneObjectLayer {
  Q_OBJECT

public:
  //! Constructor
  NpcMapSceneLayer(const QString &name, Aoba::LevelNpcComponent *mod)
    : MapSceneObjectLayer(name), mModule(mod)
  {
  }

  //! Sets the layer active
  void setActive(bool active) override;

  //! Returns the object type
  QString objectType() const override { return Aoba::LevelNpcComponent::TypeId(); }

  //! Renders the screen
  void renderOverlay(MapSceneLayerRenderInfo &render, QPainter &painter) override;

private:
  //! Creates an NPC
  void createNpc(MapSceneLayerRenderInfo &render, const Aoba::LevelNpc &npc, const Aoba::LevelNpcInstance &inst);

  //! Updates the active flags
  void updateActiveFlags(GraphicsNpcItem *npc);

  //! The module
  Aoba::LevelNpcComponent *mModule;
};
