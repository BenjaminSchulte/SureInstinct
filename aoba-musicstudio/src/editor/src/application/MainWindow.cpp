#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <editor/application/MainWindow.hpp>
#include <editor/application/DisplayConfiguration.hpp>
#include <editor/playback/AudioPlayer.hpp>
#include <editor/playback/AudioManager.hpp>
#include <editor/playback/AudioSongProcessor.hpp>
#include <editor/project/Project.hpp>
#include <editor/project/ProjectContext.hpp>
#include <editor/mml/MmlEditor.hpp>
#include <sft/song/Song.hpp>
#include <sft/song/MmlSongLoader.hpp>
#include <sft/song/MmlSongWriter.hpp>

#include "ui_MainWindow.h"

using namespace Editor;

// ----------------------------------------------------------------------------
MainWindow::MainWindow(QWidget *parent, DisplayConfiguration *displayConfig)
  : QMainWindow(parent)
  , mUi(new Ui::MainWindow)
  , mDisplayConfiguration(displayConfig)
  , mProject(nullptr)
  , mContext(nullptr)
  , mInstrumentsDockSize(200)
{
  AudioPlayer::instance();

  mUi->setupUi(this);

  mUi->splitter->setSizes({100, 400, 100});
  mUi->editorSplitter->setSizes({200, 200, 500});

  updateVisibleRegions();

  mUi->toolBarPlayback->setContextMenuPolicy(Qt::NoContextMenu); 
  mUi->instruments->setDisplayConfiguration(mDisplayConfiguration);
  mUi->sequencer->setDisplayConfiguration(mDisplayConfiguration);
  mUi->pianoRoll->setDisplayConfiguration(mDisplayConfiguration);
  mUi->mmlEditor->setDisplayConfiguration(mDisplayConfiguration);

  connect(mUi->actionNew, &QAction::triggered, this, &MainWindow::askForNewSong);
  connect(mUi->actionLoad, &QAction::triggered, this, &MainWindow::askForLoadSong);
  connect(mUi->actionSave, &QAction::triggered, this, &MainWindow::askForSaveSong);
  connect(mUi->actionSaveAs, &QAction::triggered, this, &MainWindow::askForSaveAsSong);
  connect(mUi->actionPlayPause, &QAction::toggled, this, &MainWindow::playOrPauseSong);
  connect(mUi->actionRewind, &QAction::triggered, this, &MainWindow::rewind);
  connect(mUi->actionQuit, &QAction::triggered, this, &MainWindow::close);
  connect(mUi->actionShowSequencer, &QAction::toggled, this, &MainWindow::showSequencerToggled);
  connect(mUi->actionShowCommandEditor, &QAction::toggled, this, &MainWindow::showCommandEditorToggled);
  connect(mUi->actionShowCommandPalette, &QAction::toggled, this, &MainWindow::showCommandPaletteToggled);
  
  connect(mUi->instrumentsSplitter, &QSplitter::splitterMoved, this, &MainWindow::instrumentsSplitterMoved);
  connect(mUi->splitter, &QSplitter::splitterMoved, this, &MainWindow::updateVisibleRegions);
  connect(mUi->editorSplitter, &QSplitter::splitterMoved, this, &MainWindow::updateVisibleRegions);

  connect(this, &MainWindow::projectChanged, mUi->instruments, &SampleListWidget::setProject);
  connect(this, &MainWindow::projectChanged, mUi->sequencer, &Sequencer::setProject);
  connect(this, &MainWindow::projectChanged, mUi->pianoRoll, &PianoRoll::setProject);
  connect(this, &MainWindow::projectChanged, mUi->mmlEditor, &MmlEditor::setProject);
  connect(this, &MainWindow::projectChanged, mUi->mmlCommandPalette, &MmlCommandPalette::setProject);
  connect(this, &MainWindow::projectChanged, mUi->mmlCommandEditor, &MmlCommandEditor::setProject);
  connect(mUi->mmlEditor, &MmlEditor::commandChanged, mUi->mmlCommandEditor, &MmlCommandEditor::setCommand);

  connect(AudioManager::instance()->songProcessor(), &AudioSongProcessor::songFinished, this, &MainWindow::onSongPlaybackFinished);

  resizeWidgets();
  newSong();
  //loadSong("../techdemo/assets/songs/test.mml");
  //loadSong("../techdemo/assets/songs/hopeisolation.mml");
  //loadSong("../techdemo/assets/songs/town.mml");
  //loadSong("../techdemo/assets/songs/angelfear.mml");
  //loadSong("../techdemo/assets/songs/tellyourworld.mml");
  //loadSong("../techdemo/assets/sfx/menu_select.mml");
  //loadSong("../techdemo/assets/sfx/sfx01.mml");
  //loadSong("../../assets/mana_fortress.xm");
  //loadSong("../../assets/final_fantasy.xm");
  //loadSong("../../assets/mountain.it");

  /*
  case 'SET_SONG_LOOP_OFFSET': mml += ' /'; break;
  case 'ENABLE_ECHO': mml += ' xee'; break;
  case 'DISABLE_ECHO': mml += ' xed'; break;
  case 'ENABLE_PITCH_MODULATION_TRACK': mml += ' xpe'; break;
  case 'DISABLE_PITCH_MODULATION_TRACK': mml += ' xpd'; break;
  case 'RESUME': mml += `^${delayValue(row[1])}`; break;
  case 'PAUSE': mml += `r${delayValue(row[1])}`; break;
  case 'PLAY': mml += row[1].toLowerCase().replace(/#/, '+') + delayValue(row[2]); break;
  case 'BEGIN_REPEAT_BLOCK': mml += '['; repeatStack.push(row[1]); break;
  case 'END_REPEAT_BLOCK': mml += ']' + repeatStack.pop(); break;
  case 'SET_VOLUME': mml += `v${row[1]}`; break;
  case 'SET_SAMPLE': mml += `\n\n@"${row[1].replace('sample', 'instrument')}"\n`; break;
  case 'DEC_OCTAVE': mml += `<`; break;
  case 'INC_OCTAVE': mml += `>`; break;
  case 'SET_OCTAVE': mml += `o` + (row[1] + 1); break;
  case 'SET_SPEED': mml += `t` + tempoToBpm(row[1]); break;
  case 'SET_PANNING': mml += `y` + (row[1] - 128); break;
  case 'SET_ADSR_AR': mml += `va` + (row[1]); break;
  case 'SET_ADSR_DR': mml += `vd` + (row[1]); break;
  case 'SET_ADSR_SL': mml += `vs` + (row[1]); break;
  case 'SET_ADSR_SR': mml += `vr` + (row[1]); break;
  case 'SET_PITCH': mml += `ps` + row[1]; break;
  case 'SHIFT_PITCH': mml += `pm` + row[2]; if(row[1] != 0) { console.warn('PITCH row[1]!=0: ' + row[1])} break;
  case 'EASE_PITCH': mml += `pe` + (row[1]) + (row[2] > 0 ? `+${row[2]}` : row[2]); break;
  case 'VIBRATO': mml += `pv` + (row[1]) + (row[2] > 0 ? `+${row[2]}` : row[2]); break;
  case 'TREMOLO': mml += `vt` + (row[1]) + (row[2] > 0 ? `+${row[2]}` : row[2]); break;
  case 'TREMOLO_SLOW': mml += `vts` + (row[1]) + (row[2] > 0 ? `+${row[2]}` : row[2]); break;
  case 'DISABLE_TREMOLO': mml += `vtd`; break;
  case 'DISABLE_VIBRATO': mml += `pvd`; break;
  case 'ENABLE_NOISE': mml += `xne` + row[1]; break;
  case 'REENABLE_NOISE': mml += `xne`; break;
  case 'ADD_NOISE_CLOCK': mml += `xnc` + row[1]; break;
  case 'DISABLE_NOISE': mml += `xnd`; break;

  */
}

// ----------------------------------------------------------------------------
MainWindow::~MainWindow() {
  delete mContext;
  delete mProject;
  delete mUi;
}

// ---------------------------------------------------------------------------
void MainWindow::updateVisibleRegions() {
  mUi->actionShowSequencer->setChecked(mUi->splitter->sizes()[0] > 0);

  bool bottomVisible = mUi->splitter->sizes()[2] > 0;
  mUi->actionShowCommandPalette->setChecked(bottomVisible && mUi->editorSplitter->sizes()[0] > 0);
  mUi->actionShowCommandEditor->setChecked(bottomVisible && mUi->editorSplitter->sizes()[1] > 0);
}

// ---------------------------------------------------------------------------
void MainWindow::showSequencerToggled() {
  auto sizes = mUi->splitter->sizes();
  sizes[0] = mUi->actionShowSequencer->isChecked() ? 150 : 0;
  mUi->splitter->setSizes(sizes);
}

// ---------------------------------------------------------------------------
void MainWindow::showCommandPaletteToggled() {
  auto sizes = mUi->editorSplitter->sizes();
  sizes[0] = mUi->actionShowCommandPalette->isChecked() ? 150 : 0;
  mUi->editorSplitter->setSizes(sizes);
}

// ---------------------------------------------------------------------------
void MainWindow::showCommandEditorToggled() {
  auto sizes = mUi->editorSplitter->sizes();
  sizes[1] = mUi->actionShowCommandEditor->isChecked() ? 150 : 0;
  mUi->editorSplitter->setSizes(sizes);
}

// ---------------------------------------------------------------------------
void MainWindow::onSongPlaybackFinished() {
  mUi->actionPlayPause->setChecked(false);

  AudioManager::instance()->songProcessor()->stop();
}

// ---------------------------------------------------------------------------
void MainWindow::askForNewSong() {
  if (QMessageBox::question(this, tr("New song"), tr("Do you want to revert all unsaved changes and create a new song?")) != QMessageBox::Yes) {
    return;
  }

  newSong();
}

// ---------------------------------------------------------------------------
void MainWindow::askForLoadSong() {
  QString file = QFileDialog::getOpenFileName(this, tr("Load song"), "", tr("All song (*.mml *.xm);; All files (*.*)"));
  if (file.isEmpty()) {
    return;
  }

  loadSong(file);
}

// ---------------------------------------------------------------------------
void MainWindow::askForSaveSong() {
  if (!mCurrentFileName.isEmpty()) {
    saveSong(mCurrentFileName);
  } else {
    askForSaveAsSong();
  }
}

// ---------------------------------------------------------------------------
void MainWindow::askForSaveAsSong() {
  QString file = QFileDialog::getSaveFileName(this, tr("Save song"), mCurrentFileName, tr("All song (*.mml);; All files (*.*)"));
  if (file.isEmpty()) {
    return;
  }

  saveSong(file);
}

// ---------------------------------------------------------------------------
void MainWindow::playOrPauseSong() {
  if (mUi->actionPlayPause->isChecked()) {
    AudioManager::instance()->songProcessor()->play();
  } else {
    AudioManager::instance()->songProcessor()->pause();
  }
}

// ---------------------------------------------------------------------------
void MainWindow::rewind() {
    AudioManager::instance()->songProcessor()->rewind();
}

// ---------------------------------------------------------------------------
void MainWindow::newSong() {
  AudioManager::instance()->songProcessor()->unloadSong();

  delete mContext;
  delete mProject;

  mProject = new Project();
  mContext = new ProjectContext(mProject, mUi->mmlEditor);
  emit projectChanged(mContext);
  mContext->setActiveChannel(mProject->song()->channel(0));
  mCurrentFileName = "";

  AudioManager::instance()->songProcessor()->setSong(mProject->song());
}

// ---------------------------------------------------------------------------
void MainWindow::loadSong(const QString &fileName) {
  Project *newProject = new Project();
  ProjectContext *newContext = new ProjectContext(newProject, mUi->mmlEditor);

  if (!Sft::SongLoader::loadFromFile(newProject->song(), fileName)) {
    delete newContext;
    delete newProject;

    QMessageBox::critical(this, tr("Load song"), tr("Failed to load the requested song"));
    return;
  }

  AudioManager::instance()->songProcessor()->unloadSong();
  delete mProject;
  delete mContext;

  mProject = newProject;
  mContext = newContext;

  emit projectChanged(mContext);

  mContext->setActiveChannel(mProject->song()->channel(0));
  mContext->setActiveInstrument(mProject->song()->instruments().first());
  mCurrentFileName = fileName;

  AudioManager::instance()->songProcessor()->setSong(mProject->song());
}

// ---------------------------------------------------------------------------
void MainWindow::saveSong(const QString &fileName) {
  Sft::MmlSongWriter writer(mProject->song());
  if (!writer.save(fileName)) {
    QMessageBox::critical(this, tr("Save song"), tr("Failed to save the requested song"));
    return;
  }

  mCurrentFileName = fileName;
}

// ---------------------------------------------------------------------------
void MainWindow::instrumentsSplitterMoved() {
  QList<int> sizes = mUi->instrumentsSplitter->sizes();
  mInstrumentsDockSize = sizes[1];
}

// ---------------------------------------------------------------------------
void MainWindow::resizeEvent(QResizeEvent *e) {
  QMainWindow::resizeEvent(e);
  resizeWidgets();
}

// ---------------------------------------------------------------------------
void MainWindow::resizeWidgets() {
  QList<int> sizes = mUi->instrumentsSplitter->sizes();
  int width = size().width();

  if (width < mInstrumentsDockSize) {
    mInstrumentsDockSize = 200;
  }

  if (width > mInstrumentsDockSize) {
    sizes[0] = width - mInstrumentsDockSize;
    sizes[1] = mInstrumentsDockSize;
  } else {
    sizes[0] = width;
    sizes[1] = 0;
  }

  mUi->instrumentsSplitter->setSizes(sizes);
}
