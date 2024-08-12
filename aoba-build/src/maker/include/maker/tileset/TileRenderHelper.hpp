#pragma once

#include <QPixmap>
#include <QPainter>
#include <QBrush>
#include <QVector>

class QGraphicsItem;

namespace Rpg {
struct TilesetTileCollision;
}

namespace Aoba {
class TilesetLayer;
class TilesetTile;
}

class TilesetContext;

class TileRenderHelper {
public:
  //! Constructor
  TileRenderHelper(QPixmap *pixmap);

  //! Returns the painter
  inline QPainter &painter() { return mPainter; }

  //! Renders a tile
  void renderTile(Aoba::TilesetLayer *layer, int tile, float x, float y, bool front=true, bool back=true, bool flipX=false, bool flipY=false, float zoom=1);

  //! Renders a tile
  void renderTile(Aoba::TilesetLayer *layer, Aoba::TilesetTile *tile, float x, float y, bool front=true, bool back=true, bool flipX=false, bool flipY=false, float zoom=1);

  //! Renders a tile
  void renderCollision(int x, int y, const Rpg::TilesetTileCollision &self, const Rpg::TilesetTileCollision &top, const Rpg::TilesetTileCollision &right, const Rpg::TilesetTileCollision &bottom, const Rpg::TilesetTileCollision &left);

  //! Renders priority
  void renderPriority(int x, int y, bool priority);

  void renderSwitchInactive(int x, int y);

  //! Returns a transparent brush
  static const QBrush &transparentBrush();

  //! Creates a movement icon
  static QGraphicsItem *createMovementIcon(const QPoint &, const Rpg::TilesetTileCollision &col, bool collisionIsCross);

  //! Creates a movement icon
  static QGraphicsItem *createEventIcon(const QPoint &, const Rpg::TilesetTileCollision &col);

  //! Creates a movement icon
  static QGraphicsItem *createTileIcon(const QPoint &, const QString &icon);

  //! Creates all icons
  static QVector<QGraphicsItem*> createTileIcons(const QPoint &, const Rpg::TilesetTileCollision &col, bool movement, bool event, bool exit, bool collision);

  //! Creates a tile rect
  static QGraphicsItem *createTileRect(const QPoint &, const QPen &, const QBrush &);

private:
  //! The painter
  QPainter mPainter;

  //! Whether the static brush is initialized
  static bool mTransparentBrushInitialized;

  //! Transparent brush
  static QBrush mTransparentBrush;
};