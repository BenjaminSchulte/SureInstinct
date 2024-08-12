#include <QDebug>
#include <QFileInfo>
#include <QSound>
#include <editor/sample/SampleFileDialog.hpp>
#include <editor/Configuration.hpp>

using namespace Editor;

// -----------------------------------------------------------------------------
SampleFileDialog::SampleFileDialog(QWidget *parent, const QString &directory)
  : QFileDialog(parent, tr("Load instrument"), directory, tr("All samples (*.wav *.brr *.yml);; YML Instrument (*.yml);; WAV files (*.wav);; BRR files (*.brr);; All files (*.*)"))
{
  connect(this, &QFileDialog::currentChanged, this, &SampleFileDialog::onCurrentChanged);
}

// -----------------------------------------------------------------------------
SampleFileDialog::~SampleFileDialog() {
  delete mSoundPlayback;
}

// -----------------------------------------------------------------------------
void SampleFileDialog::onCurrentChanged(const QString &path) {
  QFileInfo info(path);

  delete mSoundPlayback;
  mSoundPlayback = nullptr;

  if (!info.exists() || !info.isFile() || info.size() > EDITOR_MAX_PREVIEW_FILESIZE) {
    return;
  }

  QString ext(info.suffix().toLower());
  if (ext == "wav") {
    mSoundPlayback = new QSound(path);
    mSoundPlayback->play();
  }
}