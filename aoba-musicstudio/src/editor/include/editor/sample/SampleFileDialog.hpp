#pragma once

#include <QFileDialog>

class QSound;

namespace Editor {

class SampleFileDialog : public QFileDialog {
  Q_OBJECT

public:
  //! Constructor
  SampleFileDialog(QWidget *parent = nullptr, const QString &directory = QString());

  //! Deconsturctor
  ~SampleFileDialog();

protected slots:
  //! Current changed
  void onCurrentChanged(const QString &path);

protected:
  //! Playback for sounds
  QSound *mSoundPlayback = nullptr;
};

}