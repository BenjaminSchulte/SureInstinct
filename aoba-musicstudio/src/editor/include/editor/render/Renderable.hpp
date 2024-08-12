#pragma once

#include <QObject>
#include <QVector>
#include <QRect>
#include <QWheelEvent>

namespace Editor {
class RenderHelper;
class RenderWidget;
class DisplayConfiguration;

class Renderable : public QObject {
  Q_OBJECT

public:
  //! Constructor
  Renderable(RenderWidget *parent, const QRect &pos)
    : mParent(parent)
    , mPosition(pos)
    , mCurrentHoverRenderable(nullptr)
  {
  }

  //! Deconstructor
  virtual ~Renderable() = default;

  //! Draws this renderable
  virtual void draw(RenderHelper &) = 0;

  //! Returns the position
  const QRect &position() const { return mPosition; }

  //! Sets the position
  virtual void setPosition(const QRect &pos) { mPosition = pos; }

  //! The mouse move event
  virtual bool mouseMoveEvent(const QPoint &);

  //! The mouse move event
  virtual bool mousePressEvent(Qt::MouseButton, const QPoint &);

  //! The mouse move event
  virtual bool mouseReleaseEvent(Qt::MouseButton, const QPoint &);

  //! On double click
  virtual bool mouseDoubleClickEvent(Qt::MouseButton, const QPoint &);

  //! The mouse move event
  virtual bool mouseEnterEvent();

  //! The mouse move event
  virtual bool mouseLeaveEvent();

  //! The mouse wheel event
  virtual bool wheelEvent(QWheelEvent*);

  //! The mouse wheel event
  virtual bool zoomEvent(float) { return false; }

  //! Returns the renderable unter the mouse position
  Renderable *renderableAtPosition(const QPoint &point) const;

  //! Draws all renderables
  void drawAllRenderables(RenderHelper &helper);

  //! Returns the render widget
  inline RenderWidget *renderWidget() const { return mParent; }

protected:
  //! Returns the config
  DisplayConfiguration *config();

  //! the parent
  RenderWidget *mParent;

  //! The position
  QRect mPosition;

  //! Child renderable
  QVector<Renderable *> mRenderables;

  //! The current hovered renderable
  Renderable *mCurrentHoverRenderable;
};

}