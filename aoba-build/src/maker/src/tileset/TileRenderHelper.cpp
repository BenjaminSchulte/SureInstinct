#include <aoba/log/Log.hpp>
#include <QPainterPath>
#include <QGraphicsPixmapItem>
#include <rpg/tileset/TilesetCollisionModule.hpp>
#include <aoba/tileset/Tileset.hpp>
#include <aoba/tileset/TilesetTile.hpp>
#include <maker/tileset/TileRenderHelper.hpp>
#include <maker/tileset/TilesetContext.hpp>
#include <maker/tileset/TileRenderCache.hpp>
#include <maker/GlobalContext.hpp>

QBrush TileRenderHelper::mTransparentBrush;
bool TileRenderHelper::mTransparentBrushInitialized = false;


// -----------------------------------------------------------------------------
TileRenderHelper::TileRenderHelper(QPixmap *dest)
  : mPainter(dest)
{
}

// -----------------------------------------------------------------------------
void TileRenderHelper::renderSwitchInactive(int x, int y) {
  mPainter.fillRect(QRect(x, y, 16, 16), QBrush(QColor(255, 255, 255, 96)));
}

// -----------------------------------------------------------------------------
void TileRenderHelper::renderTile(Aoba::TilesetLayer *layer, Aoba::TilesetTile *tile, float x, float y, bool front, bool back, bool flipX, bool flipY, float zoom) {
  renderTile(layer, tile->originalIndex(), x, y, front, back, flipX, flipY, zoom);
}

// -----------------------------------------------------------------------------
void TileRenderHelper::renderTile(Aoba::TilesetLayer *layer, int tileIndex, float x, float y, bool front, bool back, bool flipX, bool flipY, float zoom) {
  if (tileIndex < 0 || tileIndex >= layer->allTiles().count()) {
    return;
  }

  QSize tileSize(layer->tileset()->assetSet()->tileSize());
  mPainter.save();
  mPainter.translate(x + (flipX ? tileSize.width() : 0), y + (flipY ? tileSize.height() : 0));
  mPainter.scale(zoom * (flipX ? -1 : 1), zoom * (flipY ? -1 : 1));

  const auto &tiles = context().tileRenderCache()->get(layer);

  if (front) {
    mPainter.drawPixmap(0, 0, tiles.frontTiles[tileIndex]);
  }
  if (back) {
    mPainter.drawPixmap(0, 0, tiles.backTiles[tileIndex]);
  }

  const QString &note = layer->allTiles()[tileIndex]->editorNote();
  if (!note.isEmpty()) {
    mPainter.drawText(0, tileSize.height(), note);
  }

  mPainter.restore();
}

// -----------------------------------------------------------------------------
void TileRenderHelper::renderCollision(int x, int y, const Rpg::TilesetTileCollision &self, const Rpg::TilesetTileCollision &top, const Rpg::TilesetTileCollision &right, const Rpg::TilesetTileCollision &bottom, const Rpg::TilesetTileCollision &left) {
  QColor color;
  switch (self.layerNumber()) {
    case 0: color = QColor(0, 0, 0, 255); break;
    case 1: color = QColor(255, 0, 0, 255); break;
    case 2: color = QColor(0, 255, 0, 255); break;
    case 3: color = QColor(255, 255, 0, 255); break;
  }

  mPainter.setPen(QPen(color, 2));

  QColor softTransparent(color);
  softTransparent.setAlphaF(0.2);
  QColor strongTransparent(color);
  strongTransparent.setAlphaF(0.5);

  if (self.isSingleLayer()) {
    mPainter.fillRect(QRect(x, y, 16, 16), QBrush(softTransparent));

    if (self.layerCollidesWith(top)) { mPainter.drawLine(x + 1, y + 1, x + 15, y + 1); }
    if (self.layerCollidesWith(bottom)) { mPainter.drawLine(x + 1, y + 15, x + 15, y + 15); }
    if (self.layerCollidesWith(left)) { mPainter.drawLine(x + 1, y + 1, x + 1, y + 15); }
    if (self.layerCollidesWith(right)) { mPainter.drawLine(x + 15, y + 1, x + 15, y + 15); }
  }

  if (self.collision[0]) { mPainter.fillRect(QRect(x, y, 8, 8), QBrush(strongTransparent)); }
  if (self.collision[1]) { mPainter.fillRect(QRect(x+8, y, 8, 8), QBrush(strongTransparent)); }
  if (self.collision[2]) { mPainter.fillRect(QRect(x, y+8, 8, 8), QBrush(strongTransparent)); }
  if (self.collision[3]) { mPainter.fillRect(QRect(x+8, y+8, 8, 8), QBrush(strongTransparent)); }

  QPainterPath path;
  switch (self.onMove) {
    case Rpg::TilesetTileCollision::DIAGONAL_COLLISION_NE:
      path.addPolygon(QPolygonF(QVector<QPointF>({QPointF(x, y), QPointF(x + 16, y), QPointF(x + 16, y + 16)})));
      mPainter.fillPath(path, strongTransparent);
      break;
    case Rpg::TilesetTileCollision::DIAGONAL_COLLISION_SE:
      path.addPolygon(QPolygonF(QVector<QPointF>({QPointF(x + 16, y), QPointF(x + 16, y + 16), QPointF(x, y + 16)})));
      mPainter.fillPath(path, strongTransparent);
      break;
    case Rpg::TilesetTileCollision::DIAGONAL_COLLISION_NW:
      path.addPolygon(QPolygonF(QVector<QPointF>({QPointF(x, y), QPointF(x + 16, y), QPointF(x, y + 16)})));
      mPainter.fillPath(path, strongTransparent);
      break;
    case Rpg::TilesetTileCollision::DIAGONAL_COLLISION_SW:
      path.addPolygon(QPolygonF(QVector<QPointF>({QPointF(x, y), QPointF(x + 16, y + 16), QPointF(x, y + 16)})));
      mPainter.fillPath(path, strongTransparent);
      break;
    default: break;
  }

  if (self.endsCollisionAt(Rpg::TilesetTileCollision::OUTLINE_TOP_LEFT, top)) { mPainter.drawLine(x + 1, y + 1, x + 8, y + 1); }
  if (self.endsCollisionAt(Rpg::TilesetTileCollision::OUTLINE_TOP_RIGHT, top)) { mPainter.drawLine(x + 8, y + 1, x + 15, y + 1); }
  if (self.endsCollisionAt(Rpg::TilesetTileCollision::OUTLINE_BOTTOM_LEFT, bottom)) { mPainter.drawLine(x + 1, y + 15, x + 8, y + 15); }
  if (self.endsCollisionAt(Rpg::TilesetTileCollision::OUTLINE_BOTTOM_RIGHT, bottom)) { mPainter.drawLine(x + 8, y + 15, x + 15, y + 15); }
  if (self.endsCollisionAt(Rpg::TilesetTileCollision::OUTLINE_LEFT_TOP, left)) { mPainter.drawLine(x + 1, y + 1, x + 1, y + 8); }
  if (self.endsCollisionAt(Rpg::TilesetTileCollision::OUTLINE_LEFT_BOTTOM, left)) { mPainter.drawLine(x + 1, y + 8, x + 1, y + 15); }
  if (self.endsCollisionAt(Rpg::TilesetTileCollision::OUTLINE_RIGHT_TOP, right)) { mPainter.drawLine(x + 15, y + 1, x + 15, y + 8); }
  if (self.endsCollisionAt(Rpg::TilesetTileCollision::OUTLINE_RIGHT_BOTTOM, right)) { mPainter.drawLine(x + 15, y + 8, x + 15, y + 15); }

  if (self.collision[0] != self.collision[2]) { mPainter.drawLine(x+1, y+8, x+8, y+8); }
  if (self.collision[1] != self.collision[3]) { mPainter.drawLine(x+8, y+8, x+15, y+8); }
  if (self.collision[0] != self.collision[1]) { mPainter.drawLine(x+8, y+1, x+8, y+8); }
  if (self.collision[2] != self.collision[3]) { mPainter.drawLine(x+8, y+8, x+8, y+15); }

  if (self.onMove == Rpg::TilesetTileCollision::DIAGONAL_COLLISION_NW || self.onMove == Rpg::TilesetTileCollision::DIAGONAL_COLLISION_SE) {
    mPainter.drawLine(x+15, y+1, x+1, y+15);
  }
  if (self.onMove == Rpg::TilesetTileCollision::DIAGONAL_COLLISION_NE || self.onMove == Rpg::TilesetTileCollision::DIAGONAL_COLLISION_SW) {
    mPainter.drawLine(x+1, y+1, x+15, y+15);
  }
}

// -----------------------------------------------------------------------------
const QBrush &TileRenderHelper::transparentBrush() {
  if (mTransparentBrushInitialized) {
    return mTransparentBrush;
  }
  mTransparentBrushInitialized = true;

  QPixmap transparentTexture(8, 8);
  transparentTexture.fill(QColor(0, 0, 0, 0));
  QPainter transparentPainter(&transparentTexture);
  transparentPainter.fillRect(QRect(0, 0, 4, 4), QBrush(QColor(0, 0, 0, 64)));
  transparentPainter.fillRect(QRect(4, 4, 4, 4), QBrush(QColor(0, 0, 0, 64)));
  mTransparentBrush.setTexture(transparentTexture);
  return mTransparentBrush;
}

// -----------------------------------------------------------------------------
void TileRenderHelper::renderPriority(int x, int y, bool priority) {
  if (!priority) {
    return;
  }

  QBrush brush(QColor(0, 0, 255, 128));
  mPainter.fillRect(QRect(QPoint(x, y), QSize(8, 8)), brush);
}

// -----------------------------------------------------------------------------
QGraphicsItem *TileRenderHelper::createMovementIcon(const QPoint &pos, const Rpg::TilesetTileCollision &col, bool collisionIsCross) {
  QString iconName;

  switch (col.onMove) {
    default: break;
    case Rpg::TilesetTileCollision::SLOW: iconName = "speed-slow"; break;
    case Rpg::TilesetTileCollision::FAST: iconName = "speed-fast"; break;
    case Rpg::TilesetTileCollision::STAIRS_UPLEFT: iconName = "stairs-left"; break;
    case Rpg::TilesetTileCollision::STAIRS_UPRIGHT: iconName = "stairs-right"; break;
    case Rpg::TilesetTileCollision::STAIRS_UPDOWN: iconName = "stairs-up"; break;
    case Rpg::TilesetTileCollision::DIAGONAL_COLLISION_SE: iconName = "cross"; break;
    case Rpg::TilesetTileCollision::DIAGONAL_COLLISION_NE: iconName = "cross"; break;
    case Rpg::TilesetTileCollision::DIAGONAL_COLLISION_SW: iconName = "cross"; break;
    case Rpg::TilesetTileCollision::DIAGONAL_COLLISION_NW: iconName = "cross"; break;
  }

  if (col.hasCollision()) {
    iconName = "cross";
  }

  if (iconName == "cross" && !collisionIsCross) {
    return nullptr;
  } else if (!iconName.isEmpty()) {
    return createTileIcon(pos, iconName);
  } else {
    return nullptr;
  }
}

// -----------------------------------------------------------------------------
QGraphicsItem *TileRenderHelper::createEventIcon(const QPoint &pos, const Rpg::TilesetTileCollision &col) {
  QString iconName;

  switch (col.onEnter) {
    default: break;
    case Rpg::TilesetTileCollision::WATER_REFLECT: iconName="reflect"; break;
    case Rpg::TilesetTileCollision::MOVE_TO_DEEP: iconName="visible-mix"; break;
    case Rpg::TilesetTileCollision::MOVE_TO_BG1: iconName="layer-down"; break;
    case Rpg::TilesetTileCollision::MOVE_TO_BG2: iconName="layer-up"; break;
    case Rpg::TilesetTileCollision::MOVE_TO_BACK: return createTileRect(pos, Qt::NoPen, QBrush(QColor(255, 0, 255, 96))); break;
    case Rpg::TilesetTileCollision::MOVE_TO_FRONT: return createTileRect(pos, Qt::NoPen, QBrush(QColor(0, 255, 255, 96))); break;
  }

  if (!iconName.isEmpty()) {
    return createTileIcon(pos, iconName);
  } else {
    return nullptr;
  }
}

// -----------------------------------------------------------------------------
QGraphicsItem *TileRenderHelper::createTileRect(const QPoint &pos, const QPen &pen, const QBrush &brush) {
  QGraphicsRectItem *rect = new QGraphicsRectItem(QRect(pos, QSize(16, 16)));
  rect->setPen(pen);
  rect->setBrush(brush);
  return rect;
}

// -----------------------------------------------------------------------------
QVector<QGraphicsItem*> TileRenderHelper::createTileIcons(const QPoint &pos, const Rpg::TilesetTileCollision &col, bool movement, bool event, bool exit, bool collision) {
  QVector<QGraphicsItem*> list;

  if (movement) {
    QGraphicsItem *result = createMovementIcon(pos, col, collision);
    if (result) {
      list.push_back(result);
    }
  }

  if (event) {
    QGraphicsItem *result = createEventIcon(pos, col);
    if (result) {
      list.push_back(result);
    }
  }

  if (exit && col.exit) {
    list.push_back(createTileIcon(pos, "circle"));
  }

  return list;
}

// -----------------------------------------------------------------------------
QGraphicsItem *TileRenderHelper::createTileIcon(const QPoint &pos, const QString &iconName) {
  QPixmap icon(":/shadow/" + iconName);

  QGraphicsPixmapItem *pixmap = new QGraphicsPixmapItem(icon);
  pixmap->setPos(QPoint(pos.x() + 2, pos.y() + 2));
  pixmap->setScale(1.0 / icon.width() * 12.0);

  return pixmap;
}