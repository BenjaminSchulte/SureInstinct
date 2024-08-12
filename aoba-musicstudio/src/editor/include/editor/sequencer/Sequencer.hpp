#pragma once

#include "../render/RenderScrollableWidget.hpp"

namespace Sft {
class Channel;
}

namespace Editor {
class Project;
class ProjectContext;
class SequencerChannel;

class Sequencer : public RenderScrollableWidget {
public:
  //! Constructor
  Sequencer(QWidget *parent = nullptr);

  //! Is renderable?
  bool isRenderable() const override;

  //! Returns the total scrollable widget size
  QSize scrollableSize() const override;

  //! Scrolls the content
  void doScroll(int, int) override;

public slots:
  //! Sets the project
  void setProject(ProjectContext *project);

  //! Sets the project
  void setChannel(Sft::Channel *channel);

  //! Sets the project
  void onCodeChanged();

  //! Audio has changed
  void onAudioTimer();

  //! Audio has changed
  void redraw();

protected:
  //! Recreates the content
  void recreateContent() override;

  //! Rendering
  void drawGL(QPainter *painter, QOpenGLFunctions *) override;

  //! The mouse move event
  void wheelEvent(QWheelEvent *e) override;

  //! The project
  Project *mProject;

  //! The context
  ProjectContext *mContext;
  
  //! The current zoom
  double mZoom = 0.5;

  //! All channels
  QVector<SequencerChannel*> mChannels;
};

}