#pragma once

#include <QMainWindow>

namespace Editor {
class Project;
class ProjectContext;
class MainEditor;
class DisplayConfiguration;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  //! Constructor
  MainWindow(QWidget *parent, DisplayConfiguration *displayConfig);

  //! Deconstructor
  ~MainWindow();

  //! Creates a new song
  void newSong();

  //! Loads a song
  void loadSong(const QString &fileName);

  //! Loads a song
  void saveSong(const QString &fileName);

signals:
  //! The song has been changed
  void projectChanged(ProjectContext*);

public slots:
  //! Creates a new song
  void askForNewSong();

  //! Creates a new song
  void askForLoadSong();

  //! Creates a new song
  void askForSaveSong();

  //! Creates a new song
  void askForSaveAsSong();

  //! Play or pause song
  void playOrPauseSong();
  
  //! The song playback is finished
  void onSongPlaybackFinished();

  //! Play or pause song
  void rewind();

  //! Updates the visible regions
  void updateVisibleRegions();

  //! Toggles the section
  void showSequencerToggled();

  //! Toggles the section
  void showCommandPaletteToggled();

  //! Toggles the section
  void showCommandEditorToggled();

protected slots:
  //! Update the dock sizes
  void instrumentsSplitterMoved();

protected:
  //! The resize event
  void resizeEvent(QResizeEvent *e);

  //! Resize the widgets
  void resizeWidgets();

  //! The UI
  Ui::MainWindow *mUi;

  //! The configuration
  DisplayConfiguration *mDisplayConfiguration;

  //! The current project
  Project *mProject;

  //! The current project context
  ProjectContext *mContext;

  //! The previous filename
  QString mCurrentFileName;

  //! The current width of the instruments "dock"
  int mInstrumentsDockSize;
};

}