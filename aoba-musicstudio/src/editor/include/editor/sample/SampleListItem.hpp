#pragma once

#include <QString>
#include <QSharedPointer>
#include "../render/RenderTheme.hpp"
#include "../render/Renderable.hpp"

namespace Sft {
class Instrument;
typedef QSharedPointer<Instrument> InstrumentPtr;
}

namespace Editor {
class RenderButton;
class ProjectContext;
class SampleListWidget;

class SampleListItem : public Renderable {
  Q_OBJECT

public:
  //! Constructor
  SampleListItem(SampleListWidget *parent, ProjectContext *context, const Sft::InstrumentPtr &instr, const QRect &pos);

  //! Draws this renderable
  void draw(RenderHelper &) override;

  //! Sets the position
  void setPosition(const QRect &pos) override;

  //! The mouse move event
  bool mousePressEvent(Qt::MouseButton, const QPoint &) override;

  //! On double click
  bool mouseDoubleClickEvent(Qt::MouseButton, const QPoint &) override;

public slots:
  //! Redraws this record
  void redraw();

  //! Shows the settings
  void showInstrumentSettings();

  //! Export sample
  void exportSample();

protected:
  //! The sample list widget
  SampleListWidget *mSampleList;

  //! The project context
  ProjectContext *mContext;
  
  //! The instrument
  Sft::InstrumentPtr mInstrument;

  //! The render button
  RenderButton *mMuteButton;
};

}