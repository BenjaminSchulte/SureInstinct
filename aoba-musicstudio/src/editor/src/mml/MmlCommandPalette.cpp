#include <QDebug>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <editor/mml/MmlEditor.hpp>
#include <editor/mml/MmlCommandPalette.hpp>
#include <editor/project/ProjectContext.hpp>

using namespace Editor;

// ----------------------------------------------------------------------------
MmlCommandPalette::MmlCommandPalette(QWidget *parent)
  : QWidget(parent)
{
  setMinimumSize(QSize(100, 100));
  setMaximumWidth(250);

  mTree = new QTreeWidget(this);
  mTree->setHeaderHidden(true);
  mTree->setColumnCount(3);
  mTree->setColumnHidden(1, true);
  mTree->setColumnHidden(2, true);

  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setMargin(0);
  layout->addWidget(mTree);
  setLayout(layout);

  connect(mTree, &QTreeWidget::itemDoubleClicked, this, &MmlCommandPalette::insertCommand);

  createActionList();
}

// ----------------------------------------------------------------------------
void MmlCommandPalette::setProject(ProjectContext *context) {
  mContext = context;
}

// ----------------------------------------------------------------------------
void MmlCommandPalette::insertCommand(QTreeWidgetItem *item) {
  QString text = item->text(1);

  if (!text.isEmpty() && mContext) {
    mContext->editor()->insertAtCursor(text);
  }
}

// ----------------------------------------------------------------------------
void MmlCommandPalette::createActionList() {
  QTreeWidgetItem *item;

  item = new QTreeWidgetItem(mTree, QStringList(tr("Playback")));
  item->setExpanded(true);
  new QTreeWidgetItem(item, QStringList({tr("Set default node length"), "l8"}));
  mCommands.insert(Sft::MmlCommandType::SET_INSTRUMENT, new QTreeWidgetItem(item, QStringList({tr("Set instrument"), "@\"id\""})));
  mCommands.insert(Sft::MmlCommandType::SET_PANNING, new QTreeWidgetItem(item, QStringList({tr("Set panning"), "y0"})));
  mCommands.insert(Sft::MmlCommandType::SET_VOLUME, new QTreeWidgetItem(item, QStringList({tr("Set volume"), "v80"})));
  mCommands.insert(Sft::MmlCommandType::SET_OCTAVE, new QTreeWidgetItem(item, QStringList({tr("Set octave"), "o5"})));
  mCommands.insert(Sft::MmlCommandType::MODIFY_OCTAVE, new QTreeWidgetItem(item, QStringList({tr("Increase/Decrease octave"), "<"})));
  mCommands.insert(Sft::MmlCommandType::PLAY_NOTE, new QTreeWidgetItem(item, QStringList({tr("Play note"), "c"})));
  mCommands.insert(Sft::MmlCommandType::RESUME_NOTE, new QTreeWidgetItem(item, QStringList({tr("Resume note"), "^"})));
  mCommands.insert(Sft::MmlCommandType::PAUSE, new QTreeWidgetItem(item, QStringList({tr("Pause"), "r"})));

  item = new QTreeWidgetItem(mTree, QStringList(tr("Song Control")));
  item->setExpanded(true);
  mCommands.insert(Sft::MmlCommandType::SET_TRACK_REPEAT_OFFSET, new QTreeWidgetItem(item, QStringList({tr("Set channel repeat"), "/"})));
  mCommands.insert(Sft::MmlCommandType::SET_TEMPO, new QTreeWidgetItem(item, QStringList({tr("Set tempo"), "t120"})));
  mCommands.insert(Sft::MmlCommandType::BEGIN_LOOP, new QTreeWidgetItem(item, QStringList({tr("Loop from here"), "["})));
  mCommands.insert(Sft::MmlCommandType::REPEAT_LOOP, new QTreeWidgetItem(item, QStringList({tr("Loop until here"), "]2"})));

  item = new QTreeWidgetItem(mTree, QStringList(tr("Effects")));
  item->setExpanded(true);
  mCommands.insert(Sft::MmlCommandType::SET_ECHO_ENABLED, new QTreeWidgetItem(item, QStringList({tr("Echo en/disable"), "xee"})));
  mCommands.insert(Sft::MmlCommandType::SET_NOISE_ENABLED, new QTreeWidgetItem(item, QStringList({tr("Noise en/disable"), "xne"})));
  mCommands.insert(Sft::MmlCommandType::SET_PITCH_CHANNEL_ENABLED, new QTreeWidgetItem(item, QStringList({tr("Pitch Channel en/disable"), "xpe"})));
  
  //PITCH_NOTE,
  //SET_ADSR_PROPERTY,
  //SET_PITCH_OFFSET,
}
