#pragma once

#include <QWidget>
#include <QVector>

namespace Maker {
class GameProject;
}

namespace Aoba {
class Character;
class Level;
class LevelNpcComponent;
}

namespace Ui {
class NpcToolWidget;
}

class NpcToolWidget : public QWidget {
  Q_OBJECT

public:
  //! Constructor
  NpcToolWidget(QWidget *parent = nullptr);

  //! Deconstructor
  ~NpcToolWidget();

signals:
  //! The NPC visual has been changed
  void visualChanged();

public slots:
  //! Sets the project
  void setProject(Maker::GameProject *project);

  //! Sets the current level
  void setLevel(Aoba::Level *level);

  //! Sets the selected NPCs
  void setSelectedNpc(const QVector<uint32_t> &);
  
private slots:
  //! Updates the list of all characters
  void updateCharacterList();

  //! Updates the list of all characters
  void updateDirectionList();

  //! The current NPC changed
  void on_npcCharacterSelect_currentIndexChanged(int);

  //! The current NPC changed
  void on_npcDirectionSelect_currentIndexChanged(int);

  //! The current NPC changed
  void on_npcForegroundCheckBox_stateChanged(int);

  //! The current NPC changed
  void on_npcCrowdIdEdit_textChanged();

  //! Updates the npc
  void updateNpc();

  //! Updates the instance
  void updateInstance();

protected:
  //! Sets the selected character
  void setSelectedCharacter(const QString &character, const QString &direction, bool front);

  //! Updates the UI
  void updateUi();

  //! Returns the current character
  Aoba::Character *currentCharacter() const;

  //! The Ui
  Ui::NpcToolWidget *mUi;

  //! Sets the selected NPC
  QVector<uint32_t> mSelection;

  //! The current project
  Maker::GameProject *mProject = nullptr;

  //! The current level
  Aoba::Level *mLevel = nullptr;

  //! The NPC component
  Aoba::LevelNpcComponent *mNpcs = nullptr;
};