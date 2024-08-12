#pragma once

#include "RenderTheme.hpp"
#include <QOpenGLWidget>

class QColor;
class QPainter;
class QOpenGLFunctions;

namespace Editor {
class DisplayConfiguration;
class Renderable;
class RenderHelper;
  
class RenderWidget : public QOpenGLWidget {
public:
  //! Constructor
  RenderWidget(QWidget *parent = nullptr);

  //! Deconstructor
  ~RenderWidget();

  //! Sets the display configuration
  void setDisplayConfiguration(DisplayConfiguration *config) {
    mConfiguration = config;
    repaint();
  }

  //! Returns whether the widget can be rendered
  virtual bool isRenderable() const;

  //! Returns the config
  inline DisplayConfiguration *displayConfiguration() const { return mConfiguration; }

protected:
  //! Returns the theme
  RenderTheme *theme() const;

  //! Returns a color
  QColor color(QPalette::ColorRole) const;

  //! Returns a color
  QImage icon(RenderTheme::IconId, int size=16) const;

  //! Initialize
  void initializeGL() override;

  //! Resize
  void resizeGL(int w, int h) override;

  //! Paint
  void paintGL() override;

  //! Recreate content
  virtual void recreateContent() {}

  //! Real paint
  virtual void drawGL(QPainter *, QOpenGLFunctions *) {};

  //! Draws all renderables
  void drawAllRenderables(RenderHelper &);

  //! Removes all renderables
  void removeAllRenderables();

  //! The mouse move event
  void mouseMoveEvent(QMouseEvent *e) override;

  //! The mouse move event
  void mousePressEvent(QMouseEvent *e) override;

  //! The mouse move event
  void mouseReleaseEvent(QMouseEvent *e) override;

  //! On double click
  void mouseDoubleClickEvent(QMouseEvent *e) override;

  //! The mouse move event
  void wheelEvent(QWheelEvent *e) override;

  //! The mouse move event
  void leaveEvent(QEvent *e) override;

  //! Returns the renderable unter the mouse position
  Renderable *renderableAtPosition(const QPoint &point) const;

  //! The display configuration
  DisplayConfiguration *mConfiguration;

  //! List of all renderables
  QVector<Renderable*> mRenderables;

  //! The current hovered renderable
  Renderable *mCurrentHoverRenderable;

  //! Whether the mouse button is currently pressed
  bool mMouseIsPressed = false;
};

}