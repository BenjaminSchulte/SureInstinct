#pragma once

#include <QScrollArea>
#include <QVector>
#include <QMap>
#include <sft/mml/MmlCommand.hpp>

class QLayout;
class QVBoxLayout;
class QVariant;

namespace Editor {
class ProjectContext;

enum class MmlCommandEditorActionType {
  CONSTANT,
  STRING,
  BOOLEAN,
  NOTE,
  DURATION,
  RANGE,
  RANGE_WITH_SIGN,
  INSTRUMENT
};

struct MmlCommandEditonActionFormat {
  MmlCommandEditonActionFormat() : type(MmlCommandEditorActionType::CONSTANT), text() {}
  MmlCommandEditonActionFormat(const QString &text) : type(MmlCommandEditorActionType::CONSTANT), text(text) {}
  MmlCommandEditonActionFormat(const QString &name, const QString &description, bool required, MmlCommandEditorActionType type) : name(name), description(description), required(required), type(type) {}
  MmlCommandEditonActionFormat(const QString &name, const QString &description, bool required, MmlCommandEditorActionType type, const QString &text) : name(name), description(description), required(required), type(type), text(text) {}
  MmlCommandEditonActionFormat(const QString &name, const QString &description, bool required, MmlCommandEditorActionType type, int from, int to) : name(name), description(description), required(required), type(type), min(from), max(to) {}

  inline bool visible() const { return type != MmlCommandEditorActionType::CONSTANT; }

  QString name;
  QString description;
  bool required;
  MmlCommandEditorActionType type;
  QString text;
  int min;
  int max;
};

struct MmlCommandEditorAction {
  MmlCommandEditorAction() {}
  MmlCommandEditorAction(const QString &name, const QString &description, const QVector<MmlCommandEditonActionFormat> &format)
    : name(name), description(description), format(format)
  {}

  QString name;
  QString description;

  QVector<MmlCommandEditonActionFormat> format;
};

class MmlCommandEditor : public QScrollArea {
  Q_OBJECT

public:
  //! Constructor
  MmlCommandEditor(QWidget *parent = nullptr);
  
public slots:
  //! The current channel has been changed
  void setProject(ProjectContext*);

  //! Sets the current command
  void setCommand(const Sft::WeakMmlCommandPtr &command);

private:
  //! Clears the layout
  void clearLayout(QLayout *layout);

  //! Creates the editor
  void recreateEditor();

  //! Creates the action list
  void createActionList();

  //! Creates the editor for the current format
  QWidget *createEditor(const MmlCommandEditonActionFormat &format, const QVariant &value);

  //! Creates the note editor
  QWidget *createBooleanEditor(const QVariant &value);

  //! Creates the note editor
  QWidget *createNoteEditor(const QVariant &value);

  //! Creates the note editor
  QWidget *createDurationEditor(const QVariant &value);

  //! Creates the note editor
  QWidget *createInstrumentEditor(const QVariant &value);

  //! Creates the note editor
  QWidget *createRangeEditor(int min, int max, const QVariant &value);

  //! Parses the next value from the input
  QVariant parseValue(const MmlCommandEditonActionFormat &format, int &offset);

  //! Parses the next value from the input
  QVariant parseNote(int &offset);

  //! Parses anumber
  QVariant parseInteger(int &offset);

  //! Parses anumber
  QVariant parseDuration(int &offset);

  //! Returns the current character
  QChar commandCharAt(int offset) const;

  //! Project context
  ProjectContext *mContext = nullptr;

  //! Command map
  QMap<Sft::MmlCommandType, MmlCommandEditorAction> mActions;

  //! The layout
  QVBoxLayout *mLayout;

  //! The current command
  Sft::WeakMmlCommandPtr mCommand;

  //! List of all editor inputs
  QMap<int, QWidget*> mEditorInstances;
};

}