#include <QDebug>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QLabel>
#include <editor/mml/MmlEditor.hpp>
#include <editor/mml/MmlCommandEditor.hpp>
#include <editor/project/ProjectContext.hpp>
#include <editor/project/Project.hpp>
#include <sft/mml/MmlParser.hpp>
#include <sft/song/Song.hpp>
#include <sft/instrument/Instrument.hpp>

using namespace Editor;

// ----------------------------------------------------------------------------
MmlCommandEditor::MmlCommandEditor(QWidget *parent)
  : QScrollArea(parent)
{
  setMinimumSize(QSize(100, 100));
  setMaximumWidth(250);

  mLayout = new QVBoxLayout(this);
  //widget->setLayout());
  setWidget(mLayout->widget());
  setWidgetResizable(true);

  createActionList();
  recreateEditor();
}

// ----------------------------------------------------------------------------
void MmlCommandEditor::setProject(ProjectContext *context) {
  mContext = context;
}

// -----------------------------------------------------------------------------
void MmlCommandEditor::setCommand(const Sft::WeakMmlCommandPtr &command) {
  mCommand = command;

  recreateEditor();
}

// ----------------------------------------------------------------------------
void MmlCommandEditor::clearLayout(QLayout *layout) {
  while (QLayoutItem* item = layout->takeAt(0)) {
    if (QWidget* widget = item->widget()) {
      widget->deleteLater();
    }
    if (QLayout* childLayout = item->layout()) {
      clearLayout(childLayout);
    }
    delete item;
  }

  mEditorInstances.clear();
}

// ----------------------------------------------------------------------------
void MmlCommandEditor::recreateEditor() {
  clearLayout(mLayout);

  QLabel *label;
  QFont font;

  //! Tests whether there is any input
  Sft::MmlCommandPtr command = mCommand.lock();
  MmlCommandEditorAction action;
  if (command) {
    action = mActions[command->type()];
  }

  if (!command || action.name.isEmpty()) {
    label = new QLabel(tr("No command selected"), widget());
    label->setWordWrap(true);
    mLayout->addWidget(label);
    mLayout->addStretch();
    return;
  }

  //! Adds name and description
  label = new QLabel(action.name, widget());
  font = label->font();
  font.setBold(true);
  label->setFont(font);
  label->setWordWrap(true);
  mLayout->addWidget(label);
  label = new QLabel(action.description, widget());
  label->setWordWrap(true);
  mLayout->addWidget(label);

  //! Adds all items
  int index = 0;
  int offset = 0;
  for (const auto &format : action.format) {
    QWidget *widget = createEditor(format, parseValue(format, offset));
    mEditorInstances.insert(index++, widget);
  }

  mLayout->addStretch();
}

// ----------------------------------------------------------------------------
QVariant MmlCommandEditor::parseValue(const MmlCommandEditonActionFormat &format, int &offset) {
  switch (format.type) {
    case MmlCommandEditorActionType::CONSTANT: offset += format.text.length(); break;
    case MmlCommandEditorActionType::NOTE:
      return parseNote(offset);
    case MmlCommandEditorActionType::DURATION:
      return parseDuration(offset);
    case MmlCommandEditorActionType::RANGE:
    case MmlCommandEditorActionType::RANGE_WITH_SIGN:
      return parseInteger(offset);

    default:
      qWarning() << "Skipping value parsing for type" << (int)format.type;
  }
  (void)format;
  (void)offset;
  return QVariant();
}

// ----------------------------------------------------------------------------
QChar MmlCommandEditor::commandCharAt(int offset) const {
  auto command = mCommand.lock();
  if (command) {
    return command->info().charAt(offset);
  } else {
    return '\0';
  }
}

// ----------------------------------------------------------------------------
QVariant MmlCommandEditor::parseNote(int &offset) {
  QString note = commandCharAt(offset++).toUpper();
  if (commandCharAt(offset) == '+') {
    note += "+";
    offset++;
  }
  return note;
}

// ----------------------------------------------------------------------------
QVariant MmlCommandEditor::parseDuration(int &offset) {
  if (commandCharAt(offset) == ':') {
    offset++;
    return parseInteger(offset);
  }

  QVariant number = parseInteger(offset);
  return Sft::MmlParser::mmlDurationToDuration(number.toInt());
}

// ----------------------------------------------------------------------------
QVariant MmlCommandEditor::parseInteger(int &offset) {
  bool negative = false;
  QChar first = commandCharAt(offset);
  if (first == '+') {
    offset++;
  } else if (first == '-') {
    negative = true;
    offset++;
  }

  QString text;
  do {
    QChar digit = commandCharAt(offset);
    if (digit < '0' || digit > '9') {
      break;
    }

    text += digit;
    offset++;
  } while(true);

  int number = text.toInt();
  if (negative) {
    number = -number;
  }

  return number;
}

// ----------------------------------------------------------------------------
QWidget *MmlCommandEditor::createEditor(const MmlCommandEditonActionFormat &format, const QVariant &value) {
  if (!format.visible()) {
    return nullptr;
  }

  QLabel *label = new QLabel(format.name, widget());
  QFont font(label->font());
  font.setBold(true);
  label->setFont(font);
  label->setWordWrap(true);
  mLayout->addWidget(label);

  if (!format.description.isEmpty()) {
    label = new QLabel(format.description, widget());
    label->setWordWrap(true);
    mLayout->addWidget(label);
  }

  QWidget *widget = nullptr;

  switch (format.type) {
    case MmlCommandEditorActionType::STRING:
      break;

    case MmlCommandEditorActionType::BOOLEAN:
      widget = createBooleanEditor(value);
      break;

    case MmlCommandEditorActionType::NOTE:
      widget = createNoteEditor(value);
      break;

    case MmlCommandEditorActionType::DURATION:
      widget = createDurationEditor(value);
      break;

    case MmlCommandEditorActionType::INSTRUMENT:
      widget = createInstrumentEditor(value);
      break;

    case MmlCommandEditorActionType::RANGE:
    case MmlCommandEditorActionType::RANGE_WITH_SIGN:
      widget = createRangeEditor(format.min, format.max, value);
      break;

    default:
      break;
  }

  mLayout->addWidget(widget);

  return widget;
}

// ----------------------------------------------------------------------------
QWidget *MmlCommandEditor::createNoteEditor(const QVariant &value) {
  QComboBox *edit = new QComboBox(widget());
  edit->addItem("C");
  edit->addItem("C+");
  edit->addItem("D");
  edit->addItem("D+");
  edit->addItem("E");
  edit->addItem("F");
  edit->addItem("F+");
  edit->addItem("G");
  edit->addItem("G+");
  edit->addItem("A");
  edit->addItem("A+");
  edit->addItem("B");

  edit->setCurrentIndex(edit->findText(value.toString()));

  return edit;
}

// ----------------------------------------------------------------------------
QWidget *MmlCommandEditor::createBooleanEditor(const QVariant &value) {
  (void)value;
  
  QCheckBox *edit = new QCheckBox(widget());
  return edit;
}

// ----------------------------------------------------------------------------
QWidget *MmlCommandEditor::createDurationEditor(const QVariant &value) {
  QSpinBox *edit = new QSpinBox(widget());
  edit->setRange(1, 192);
  edit->setSingleStep(1);
  edit->setValue(value.toInt());
  return edit;
}

// ----------------------------------------------------------------------------
QWidget *MmlCommandEditor::createRangeEditor(int min, int max, const QVariant &value) {
  QSpinBox *edit = new QSpinBox(widget());
  edit->setRange(min, max);
  edit->setSingleStep(1);
  edit->setValue(value.toInt());
  return edit;
}

// ----------------------------------------------------------------------------
QWidget *MmlCommandEditor::createInstrumentEditor(const QVariant &value) {
  (void)value;
  
  QComboBox *edit = new QComboBox(widget());
  for (const Sft::InstrumentPtr &instrument : mContext->project()->song()->instruments().all()) {
    edit->addItem(instrument->name());
  }
  return edit;
}

// ----------------------------------------------------------------------------
void MmlCommandEditor::createActionList() {
  mActions.insert(Sft::MmlCommandType::SET_ECHO_ENABLED, MmlCommandEditorAction(
    tr("En/Disable Echo"),
    tr("Enables or disables the SNES echo for this channel"),
    {
      MmlCommandEditonActionFormat("xe"),
      MmlCommandEditonActionFormat(tr("Enabled"), "", false, MmlCommandEditorActionType::BOOLEAN)
    }
  ));
  mActions.insert(Sft::MmlCommandType::SET_PITCH_CHANNEL_ENABLED, MmlCommandEditorAction(
    tr("En/Disable Pitch Channel"),
    tr("TODO - Description missing"),
    {
      MmlCommandEditonActionFormat("xp"),
      MmlCommandEditonActionFormat(tr("Enabled"), "", false, MmlCommandEditorActionType::BOOLEAN)
    }
  ));
  mActions.insert(Sft::MmlCommandType::SET_TEMPO, MmlCommandEditorAction(
    tr("Set Tempo"),
    tr("Sets the global song tempo"),
    {
      MmlCommandEditonActionFormat("t"),
      MmlCommandEditonActionFormat(tr("BPM"), "", false, MmlCommandEditorActionType::RANGE, 39, 10000)
    }
  ));
  mActions.insert(Sft::MmlCommandType::SET_VOLUME, MmlCommandEditorAction(
    tr("Set Volume"),
    tr("Sets the volume for the current channel"),
    {
      MmlCommandEditonActionFormat("v"),
      MmlCommandEditonActionFormat(tr("Volume"), "", false, MmlCommandEditorActionType::RANGE, 0, 127)
    }
  ));
  mActions.insert(Sft::MmlCommandType::SET_PANNING, MmlCommandEditorAction(
    tr("Set Panning"),
    tr("Sets the panning for the current channel"),
    {
      MmlCommandEditonActionFormat("y"),
      MmlCommandEditonActionFormat(tr("Panning"), "", false, MmlCommandEditorActionType::RANGE, -127, 127)
    }
  ));
  mActions.insert(Sft::MmlCommandType::SET_OCTAVE, MmlCommandEditorAction(
    tr("Set Octave"),
    tr("Sets the octave for the current channel"),
    {
      MmlCommandEditonActionFormat("o"),
      MmlCommandEditonActionFormat(tr("Octave"), "", false, MmlCommandEditorActionType::RANGE, Sft::FIRST_OCTAVE, Sft::LAST_OCTAVE)
    }
  ));
  mActions.insert(Sft::MmlCommandType::SET_INSTRUMENT, MmlCommandEditorAction(
    tr("Set Instrument"),
    tr("Sets the instrument for the current channel"),
    {
      MmlCommandEditonActionFormat("@"),
      MmlCommandEditonActionFormat(tr("Instrument"), "", false, MmlCommandEditorActionType::INSTRUMENT)
    }
  ));
  mActions.insert(Sft::MmlCommandType::PLAY_NOTE, MmlCommandEditorAction(
    tr("Play note"),
    tr("Plays a note for a given duration"),
    {
      MmlCommandEditonActionFormat(tr("Note"), "", true, MmlCommandEditorActionType::NOTE),
      MmlCommandEditonActionFormat(tr("Duration"), "", false, MmlCommandEditorActionType::DURATION)
    }
  ));
  mActions.insert(Sft::MmlCommandType::RESUME_NOTE, MmlCommandEditorAction(
    tr("Resume note"),
    tr("Resumes the current note for a given duration"),
    {
      MmlCommandEditonActionFormat("^"),
      MmlCommandEditonActionFormat(tr("Duration"), "", false, MmlCommandEditorActionType::DURATION)
    }
  ));
  mActions.insert(Sft::MmlCommandType::PAUSE, MmlCommandEditorAction(
    tr("Pause"),
    tr("Pauses playback for a given duration"),
    {
      MmlCommandEditonActionFormat("r"),
      MmlCommandEditonActionFormat(tr("Duration"), "", false, MmlCommandEditorActionType::DURATION)
    }
  ));
  mActions.insert(Sft::MmlCommandType::SET_TRACK_REPEAT_OFFSET, MmlCommandEditorAction(
    tr("Set channel repeat"),
    tr("Repeats the channel from this position at the end"),
    {
      MmlCommandEditonActionFormat("/")
    }
  ));
  mActions.insert(Sft::MmlCommandType::BEGIN_LOOP, MmlCommandEditorAction(
    tr("Begin loop"),
    tr("Starts a repeatable pattern from this point"),
    {
      MmlCommandEditonActionFormat("[")
    }
  ));
  mActions.insert(Sft::MmlCommandType::REPEAT_LOOP, MmlCommandEditorAction(
    tr("Repeat loop"),
    tr("Repeats the loop from the start position"),
    {
      MmlCommandEditonActionFormat("]"),
      MmlCommandEditonActionFormat(tr("Repeat times"), "", false, MmlCommandEditorActionType::RANGE, 1, 255)
    }
  ));
  mActions.insert(Sft::MmlCommandType::PITCH_NOTE, MmlCommandEditorAction(
    tr("Ease Pitch"),
    tr("Eases the pitch to a relative note"),
    {
      MmlCommandEditonActionFormat("pe"),
      MmlCommandEditonActionFormat(tr("Duration"), "", false, MmlCommandEditorActionType::DURATION),
      MmlCommandEditonActionFormat(tr("Notes to ease"), "", false, MmlCommandEditorActionType::RANGE_WITH_SIGN, -127, 127)
    }
  ));

}
