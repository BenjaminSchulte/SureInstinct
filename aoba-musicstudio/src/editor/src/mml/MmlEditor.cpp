#include <QDebug>
#include <QTextCursor>
#include <QVBoxLayout>
#include <editor/mml/MmlEditor.hpp>
#include <editor/mml/MmlHighlighter.hpp>
#include <editor/mml/MmlCodeTextEdit.hpp>
#include <editor/project/ProjectContext.hpp>
#include <editor/playback/AudioPlayer.hpp>
#include <sft/channel/Channel.hpp>
#include <sft/mml/MmlScript.hpp>
#include <sft/mml/MmlExecutedScript.hpp>
#include <sft/mml/MmlPlayNoteCommand.hpp>

using namespace Editor;

// ----------------------------------------------------------------------------
MmlEditor::MmlEditor(QWidget *parent)
  : QWidget(parent)
  , mCodeEditor(new MmlCodeTextEdit(this))
{
  setMinimumSize(QSize(100, 100));

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->addWidget(mCodeEditor);
  layout->setMargin(0);
  setLayout(layout);

  mHighlighter = new MmlHighlighter(mCodeEditor->document());

  connect(mCodeEditor, &MmlCodeTextEdit::textChanged, this, &MmlEditor::onTextChanged);
  connect(mCodeEditor, &MmlCodeTextEdit::cursorPositionChanged, this, &MmlEditor::onCursorChanged);
  connect(mCodeEditor, &MmlCodeTextEdit::selectionChanged, this, &MmlEditor::onCursorChanged);
  connect(mCodeEditor->document(), &QTextDocument::contentsChange, this, &MmlEditor::onContentsChanged);
}

// ----------------------------------------------------------------------------
void MmlEditor::insertCode(const QString &text, int pos) {
  QString oldText = mCodeEditor->toPlainText();
  if (pos == -1 || pos > oldText.length()) {
    pos = oldText.length();
  }

  mPlayNoteAtOffset = -1;
  mCodeEditor->setPlainText(oldText.left(pos) + text + oldText.mid(pos));
}

// ----------------------------------------------------------------------------
void MmlEditor::onCursorChanged() {
  if (!mChannel) {
    return;
  }

  mChannel->script()->requireCompiled();
  Sft::MmlCommandPtr command = mChannel->script()->compiled()->commandAtOffset(mCodeEditor->textCursor().selectionStart());

  if (command == mCurrentCommand) {
    return;
  }

  mCurrentCommand = command;
  emit commandChanged(mCurrentCommand);
}

// ----------------------------------------------------------------------------
void MmlEditor::insertAtCursor(const QString &text) {
  mPlayNoteAtOffset = -1;
  mCodeEditor->textCursor().insertText(text);
}

// ----------------------------------------------------------------------------
int MmlEditor::getSelectionStart() const {
  return mCodeEditor->textCursor().selectionStart();
}

// ----------------------------------------------------------------------------
int MmlEditor::getSelectionEnd() const {
  return mCodeEditor->textCursor().selectionEnd();
}

// ----------------------------------------------------------------------------
QString MmlEditor::getCode() const {
  return mCodeEditor->toPlainText();
}

// ----------------------------------------------------------------------------
void MmlEditor::setDisplayConfiguration(DisplayConfiguration *config) {
  mCodeEditor->setDisplayConfiguration(config);
}

// ----------------------------------------------------------------------------
void MmlEditor::setProject(ProjectContext *context) {
  mPlayNoteAtOffset = -1;
  connect(context, &ProjectContext::activeChannelChanged, this, &MmlEditor::setChannel);
  connect(this, &MmlEditor::codeChanged, context, &ProjectContext::notifyCodeChanged);
  setChannel(context->activeChannel());
  onCursorChanged();
}

// ----------------------------------------------------------------------------
void MmlEditor::setChannel(Sft::Channel *channel) {
  mChannel = channel;

  mProtectTextChange = true;
  if (channel) {
    mCodeEditor->setPlainText(channel->script()->code());
  } else {
    mCodeEditor->setPlainText("");
  }
  onCursorChanged();
  mPlayNoteAtOffset = -1;
  mProtectTextChange = false;
}

// ----------------------------------------------------------------------------
void MmlEditor::onContentsChanged(int position, int, int charsAdded) {
  if (mProtectTextChange || !mChannel || charsAdded == 0) {
    return;
  }

  mPlayNoteAtOffset = position;
}

// ----------------------------------------------------------------------------
void MmlEditor::onTextChanged() {
  if (mProtectTextChange || !mChannel) {
    return;
  }

  mChannel->script()->set(mCodeEditor->toPlainText());
  mChannel->script()->compile();
  emit codeChanged();
  onCursorChanged();

  if (mPlayNoteAtOffset >= 0) {
    int index = mChannel->script()->executed()->commandIndexForCodeOffset(mPlayNoteAtOffset);
    if (index >= 0 && index < (int)mChannel->script()->executed()->numCommands()) {
      const auto &command = mChannel->script()->executed()->command(index);
      if (command.command() && command.command()->type() == Sft::MmlCommandType::PLAY_NOTE) {
        QSharedPointer<Sft::MmlPlayNoteCommand> playNote = command.command().dynamicCast<Sft::MmlPlayNoteCommand>();
        
        if (playNote->info().start <= mPlayNoteAtOffset && playNote->info().end >= mPlayNoteAtOffset && command.state().instrument) {
          AudioPlayer::instance()->play(command.state().instrument, playNote->note() + command.state().octave * Sft::NUM_NOTES_PER_OCTAVE, command.state().volume / 127.0, mChannel->index());
        }
      }
    }
  }

  mPlayNoteAtOffset = -1;
}
