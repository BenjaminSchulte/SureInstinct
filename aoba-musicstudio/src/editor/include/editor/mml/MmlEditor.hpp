#pragma once

#include <QWidget>
#include <QWeakPointer>

namespace Sft {
class Channel;
typedef QWeakPointer<class MmlCommand> WeakMmlCommandPtr;
}

namespace Editor {
class MmlCodeTextEdit;
class DisplayConfiguration;
class ProjectContext;
class MmlHighlighter;

class MmlEditor : public QWidget {
  Q_OBJECT

public:
  //! Constructor
  MmlEditor(QWidget *parent = nullptr);

  //! Sets the display configuration
  void setDisplayConfiguration(DisplayConfiguration *config);

  //! Inserts code to the text
  void insertCode(const QString &text, int pos=-1);

  //! Inserts code to the text
  void insertAtCursor(const QString &text);

  //! Returns the selection start
  int getSelectionStart() const;

  //! Returns the selection start
  int getSelectionEnd() const;

  //! Returns the text
  QString getCode() const;

signals:
  //! The code has been changed
  void codeChanged();

  //! The current command has been changed
  void commandChanged(const Sft::WeakMmlCommandPtr &);

private slots:
  //! Cursor has changed
  void onCursorChanged();

  //! The content has been changed
  void onContentsChanged(int position, int charsRemoved, int charsAdded);

public slots:
  //! The current channel has been changed
  void setProject(ProjectContext*);

  //! The current channel has been changed
  void setChannel(Sft::Channel*);

  //! Code has been changed
  void onTextChanged();

private:
  //! The text editor
  MmlCodeTextEdit *mCodeEditor;

  //! Highlighter
  MmlHighlighter *mHighlighter = nullptr;

  //! The channel
  Sft::Channel *mChannel = nullptr;

  //! Protect the text change
  bool mProtectTextChange = false;

  //! Current command
  Sft::WeakMmlCommandPtr mCurrentCommand;

  //! Plays a note at a given offset
  int mPlayNoteAtOffset = -1;
};

}