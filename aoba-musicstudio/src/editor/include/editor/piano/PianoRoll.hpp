#pragma once

#include "../render/RenderScrollableWidget.hpp"

namespace Sft {
class Channel;
}

namespace Editor {
class Project;
class ProjectContext;
class PianoRollPiano;
class PianoRollNoteEditor;
class PianoRollSequenceHeader;

class PianoRoll : public RenderScrollableWidget {
public:
  //! Constructor
  PianoRoll(QWidget *parent = nullptr);

  //! Deconstructor
  ~PianoRoll();

  //! Is renderable?
  bool isRenderable() const override;

  //! Returns the total scrollable widget size
  QSize scrollableSize() const override;

  //! Scrolls the content
  void doScroll(int, int) override;

  //! Zoom event
  void zoomEvent(float dir);

public slots:
  //! Sets the project
  void setProject(ProjectContext *project);

  //! Sets the channel
  void setChannel(Sft::Channel *channel);

  //! Editor scroll has changed
  void onEditorScrollChanged(float note);

  //! Code has been changed
  void onCodeChanged();

  //! Audio has changed
  void onAudioTimer();

protected:
  //! The total height of this roll
  int totalHeight() const;

  //! Rendering
  void drawGL(QPainter *painter, QOpenGLFunctions *) override;

  //! Recreate content
  void recreateContent() override;

  //! The project
  Project *mProject;

  //! The context
  ProjectContext *mContext;

  //! The piano
  PianoRollNoteEditor *mNoteEditor;

  //! The sequence header
  PianoRollSequenceHeader *mSequenceHeader;
 
  //! The piano
  PianoRollPiano *mPiano;
  
  //! The current zoom
  double mZoom = 1.0;

  //! Whether to promote scroll event
  bool mPromoteScrollEvent = true;
};

}