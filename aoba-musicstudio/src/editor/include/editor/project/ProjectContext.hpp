#pragma once

#include <QObject>
#include <QSharedPointer>

namespace Sft {
class Instrument;
typedef QSharedPointer<Instrument> InstrumentPtr;
class Channel;
}

namespace Editor {
class Project;
class MmlEditor;

class ProjectContext : public QObject {
  Q_OBJECT

public:
  //! Constructor
  ProjectContext(Project *project, MmlEditor *editor) : mProject(project), mEditor(editor) {}

  //! Returns the project
  inline Project *project() const { return mProject; }

  //! Returns the editor
  inline MmlEditor *editor() const { return mEditor; }

  //! Returns the active instrument
  inline const Sft::InstrumentPtr &activeInstrument() const { return mInstrument; }

  //! Returns the active channel
  inline Sft::Channel *activeChannel() const { return mChannel; }

  //! Returns the editor window
  inline float editorScroll() const { return mEditorWindowLeft; }

  //! Returns the editor window
  inline float editorWidth() const { return mEditorWindowWidth; }

public slots:
  //! Sets the active instrument
  void setActiveInstrument(const Sft::InstrumentPtr &);

  //! Sets the active channel
  void setActiveChannel(Sft::Channel *);

  //! Notifies about code change
  void notifyCodeChanged();

  //! The editor has been changed
  void setEditorScroll(float leftNote);

  //! The editor has been changed
  void setEditorWidth(float numNotes);

signals:
  //! The instrument has been changed
  void activeInstrumentChanged(const Sft::InstrumentPtr &);

  //! The channel has been changed
  void activeChannelChanged(Sft::Channel *);

  //! Code changed
  void codeChanged();

  //! The editor has been changed
  void editorScrollChanged(float leftNote);

  //! The editor has been changed
  void editorWidthChanged(float numNotes);
  
protected:
  //! The project
  Project *mProject;

  //! The editor
  MmlEditor *mEditor;

  //! The current instrument
  Sft::InstrumentPtr mInstrument;

  //! The current channel
  Sft::Channel *mChannel = nullptr;

  //! The editor window
  float mEditorWindowLeft = 0;

  //! The editor window
  float mEditorWindowWidth = 0;
};

}