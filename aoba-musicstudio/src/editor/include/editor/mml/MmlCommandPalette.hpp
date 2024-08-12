#pragma once

#include <QWidget>
#include <QMap>
#include <sft/mml/MmlCommand.hpp>

class QTreeWidget;
class QTreeWidgetItem;

namespace Editor {
class ProjectContext;

class MmlCommandPalette : public QWidget {
  Q_OBJECT

public:
  //! Constructor
  MmlCommandPalette(QWidget *parent = nullptr);
  
public slots:
  //! The current channel has been changed
  void setProject(ProjectContext*);

  //! Inserts a command
  void insertCommand(QTreeWidgetItem *item);

private:
  //! Creates the action list
  void createActionList();

  //! The tree list
  QTreeWidget *mTree;

  //! List of all items
  QMap<Sft::MmlCommandType, QTreeWidgetItem*> mCommands;

  //! Project context
  ProjectContext *mContext = nullptr;
};

}