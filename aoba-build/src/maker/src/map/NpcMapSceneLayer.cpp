#include <QDebug>
#include <QGraphicsRectItem>
#include <maker/map/MapScene.hpp>
#include <maker/map/NpcMapSceneLayer.hpp>
#include <maker/game/GameCharacterAssetType.hpp>
#include <maker/game/GameSpriteAssetType.hpp>
#include <maker/game/GameLevelAssetType.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/project/MakerProject.hpp>
#include <maker/GlobalContext.hpp>
#include <aoba/level/LevelNpcComponent.hpp>

// -----------------------------------------------------------------------------
GraphicsNpcItem::GraphicsNpcItem(Maker::GameProject *project, Aoba::Level *level, Aoba::LevelNpcComponent *mod, uint32_t npcId, QGraphicsItem *parent)
  : QGraphicsItem(parent)
  , mProject(project)
  , mLevel(level)
  , mNpcModule(mod)
  , mNpcId(npcId)
{
  setFlags(QGraphicsItem::ItemSendsGeometryChanges);
}

// -----------------------------------------------------------------------------
QRectF GraphicsNpcItem::boundingRect() const {
  return QRectF(-1, -1, 18, 18);
}

// -----------------------------------------------------------------------------
void GraphicsNpcItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) {
  const Aoba::LevelNpc *npc;
  const Aoba::LevelNpcInstance *instance;

  painter->fillRect(0, 0, 16, 16, QBrush(QColor(0, 0, 0, 128)));
  painter->setPen(QPen(QColor(0, 0, 0, 255), 1));
  painter->drawRect(-1, -1, 18, 18);

  if (findNpc(&npc, &instance)) {
    Maker::GameCharacterAsset *gameCharacter = dynamic_cast<Maker::GameCharacterAsset*>(mProject->assets().getAsset("Character", npc->mCharacter));

    if (mCachedCharacter != gameCharacter) {
      mCachedCharacter = gameCharacter;
      mPixmap = QPixmap();

      if (gameCharacter) {
        Maker::GameSpriteAsset *gameSprite = gameCharacter->sprite();
        
        if (gameSprite) {
          const QPixmap *image = gameSprite->previewImage();
          
          if (image) {
            mPixmap = QPixmap(14, 14);
            mPixmap.fill(QColor(0, 0, 0, 0));
            QPainter targetPainter(&mPixmap);
            targetPainter.drawPixmap(QPoint(0, 0), *image);
          }
        }
      }
    }

    if (!mPixmap.isNull()) {
      painter->drawPixmap(1, 1, mPixmap);
    }
  }

  if (isSelected()) {
    painter->setPen(QPen(QColor(255, 255, 0, 255), 1));
  } else {
    painter->setPen(QPen(QColor(255, 255, 255, 255), 1));
  }
  painter->drawRect(0, 0, 16, 16);
}

// -----------------------------------------------------------------------------
bool GraphicsNpcItem::findNpc(const Aoba::LevelNpc **npc, const Aoba::LevelNpcInstance **instance) const {
  return mNpcModule->findNpc(mLevel, mNpcId, npc, instance);
}

// -----------------------------------------------------------------------------
QVariant GraphicsNpcItem::itemChange(GraphicsItemChange change, const QVariant &value) {
  if (change == ItemPositionChange) {
    QPointF pos = value.toPointF();
    pos.setX((int)((pos.x() + 4) / 8) * 8);
    pos.setY((int)((pos.y() + 4) / 8) * 8);
    return QGraphicsItem::itemChange(change, pos);
  } else if (change == ItemPositionHasChanged) {
    QPointF pos = value.toPointF();
    int newX = pos.x() / 8;
    int newY = pos.y() / 8;

    const Aoba::LevelNpc *npc;
    const Aoba::LevelNpcInstance *inst;

    if (mNpcModule->findNpc(mLevel, mNpcId, &npc, &inst)) {
      if (inst->mX != newX || inst->mY != newY) {
        Aoba::LevelNpcInstance instCopy(*inst);
        instCopy.mX = pos.x() / 8;
        instCopy.mY = pos.y() / 8;
        qDebug() << "TODO: Make this an UNDO command!!";
        mNpcModule->setNpcInstance(mLevel, mNpcId, instCopy);
      }
    }
  }

  return QGraphicsItem::itemChange(change, value);
}


// -----------------------------------------------------------------------------
void NpcMapSceneLayer::renderOverlay(MapSceneLayerRenderInfo &render, QPainter &painter) {
  clearSceneItems();

  Aoba::LevelNpcList list = mModule->listFor(render.mapScene()->level()->asset());
  for (const Aoba::LevelNpc &npc : list.mNpcList) {
    for (const Aoba::LevelNpcInstance &inst : npc.mInstances) {
      createNpc(render, npc, inst);
    }
  }

  (void)painter;
}

// -----------------------------------------------------------------------------
void NpcMapSceneLayer::createNpc(MapSceneLayerRenderInfo &render, const Aoba::LevelNpc &, const Aoba::LevelNpcInstance &inst) {
  GraphicsNpcItem *item = new GraphicsNpcItem(
    context().project()->gameProject(),
    render.mapScene()->level()->asset(),
    mModule,
    inst.mGlobalIdentifier
  );

  item->setPos(QPoint(inst.mX *8, inst.mY * 8));

  updateActiveFlags(item);
  addSceneItem(render, item);
}

// -----------------------------------------------------------------------------
void NpcMapSceneLayer::setActive(bool active) {
  MapSceneObjectLayer::setActive(active);

  for (QGraphicsItem *item : mSceneItems) {
    updateActiveFlags(dynamic_cast<GraphicsNpcItem*>(item));
  }
}

// -----------------------------------------------------------------------------
void NpcMapSceneLayer::updateActiveFlags(GraphicsNpcItem *npc) {
  if (npc == nullptr) {
    return;
  }

  npc->setFlag(QGraphicsItem::ItemIsMovable, mActive);
  npc->setFlag(QGraphicsItem::ItemIsSelectable, mActive);
  npc->setFlag(QGraphicsItem::ItemIsFocusable, mActive);
}