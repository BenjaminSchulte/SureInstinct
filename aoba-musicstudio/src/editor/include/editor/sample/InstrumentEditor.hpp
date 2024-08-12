#pragma once

#include <QSharedPointer>
#include <QDialog>

namespace Sft {
class Instrument;
typedef QSharedPointer<Instrument> InstrumentPtr;
}

namespace Editor {
namespace Ui {
class InstrumentEditor;
}

class InstrumentEditor : public QDialog {
  Q_OBJECT
  
public:
  //! Constructor
  InstrumentEditor(QWidget *parent=nullptr);

  //! Deconstructor
  ~InstrumentEditor();

  //! Sets the current instrument
  void setInstrument(const Sft::InstrumentPtr &instrument);

  //! Plays a preview
  void playPreview();

signals:
  //! The instrument name has been modified
  void displayChanged();

protected slots:

  void updateSampleSize();

  //! The attack has been changed
  void updateAdsrValue();

  //! The attack has been changed
  void updateVolume();

  //! The attack has been changed
  void updateLoopStart();

  void updateLoopStartBlock();

  //! The attack has been changed
  void updateLooped();

  //! The attack has been changed
  void updateNoteAdjust();

  //! The attack has been changed
  void updateSampleEnd();

  //! The attack has been changed
  void updateQuality();

  //! The attack has been changed
  void updateName();

protected:
  //! The UI
  Ui::InstrumentEditor *mUi;

  //! The current instrument
  Sft::InstrumentPtr mInstrument;

  //! Whether to prevent updated
  bool mPreventUpdate = false;
};

}