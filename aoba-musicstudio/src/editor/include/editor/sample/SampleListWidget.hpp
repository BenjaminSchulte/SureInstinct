#pragma once

#include <QSharedPointer>
#include "../render/RenderScrollableWidget.hpp"

namespace Sft {
class Instrument;
typedef QSharedPointer<Instrument> InstrumentPtr;
}

namespace Editor {
class RenderButton;
class Project;
class ProjectContext;

class SampleListWidget : public RenderScrollableWidget {
  Q_OBJECT

public:
  //! Constructor
  SampleListWidget(QWidget *parent = nullptr);

  //! Returns whether the widget can be rendered
  bool isRenderable() const override;

  //! Scrolls the content
  void doScroll(int x, int y) override;

public slots:
  //! Sets the project
  void setProject(ProjectContext *project);

  //! Sets the current instrument
  void setCurrentInstrument(const Sft::InstrumentPtr &instrument);

  //! Adds a new instrument
  void addInstrumentDialog();

  //! Returns the scrollable size
  QSize scrollableSize() const override;

signals:
  //! Instrument changed
  void instrumentChanged(const Sft::InstrumentPtr &instrument);

protected:
  //! Recreates all buttons
  void recreateContent() override;

  //! Initialize
  void drawGL(QPainter *, QOpenGLFunctions *f) override;

  //! The project
  Project *mProject;

  //! The context
  ProjectContext *mContext;

  //! The ADD button
  RenderButton *mAddButton;

  //! The current instrument
  Sft::InstrumentPtr mCurrentInstrument;
};

}