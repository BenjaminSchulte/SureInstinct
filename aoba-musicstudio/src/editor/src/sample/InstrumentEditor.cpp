#include <QDebug>
#include <editor/sample/InstrumentEditor.hpp>
#include <editor/render/RenderHelper.hpp>
#include <sft/instrument/Instrument.hpp>
#include <sft/sample/Sample.hpp>
#include <sft/brr/BrrSample.hpp>

#include "ui_InstrumentEditor.h"

using namespace Editor;

// -----------------------------------------------------------------------------
InstrumentEditor::InstrumentEditor(QWidget *parent)
  : QDialog(parent)
  , mUi(new Editor::Ui::InstrumentEditor)
{
  mUi->setupUi(this);

  mUi->instrumentColor->setStandardColors();

  connect(mUi->instrumentAttack, &QSlider::valueChanged, this, &InstrumentEditor::updateAdsrValue);
  connect(mUi->instrumentDecay, &QSlider::valueChanged, this, &InstrumentEditor::updateAdsrValue);
  connect(mUi->instrumentSustain, &QSlider::valueChanged, this, &InstrumentEditor::updateAdsrValue);
  connect(mUi->instrumentRelease, &QSlider::valueChanged, this, &InstrumentEditor::updateAdsrValue);
  connect(mUi->instrumentName, &QLineEdit::textChanged, this, &InstrumentEditor::updateName);
  connect(mUi->instrumentNoteAdjust, SIGNAL(valueChanged(double)), this, SLOT(updateNoteAdjust()));
  connect(mUi->instrumentLoopStart, SIGNAL(valueChanged(double)), this, SLOT(updateLoopStart()));
  connect(mUi->instrumentLoopStartBlock, SIGNAL(valueChanged(int)), this, SLOT(updateLoopStartBlock()));
  connect(mUi->instrumentLooped, &QCheckBox::toggled, this, &InstrumentEditor::updateLooped);
  connect(mUi->instrumentVolume, SIGNAL(valueChanged(double)), this, SLOT(updateVolume()));
  connect(mUi->sampleQuality, SIGNAL(valueChanged(double)), this, SLOT(updateQuality()));
  connect(mUi->buttons, &QDialogButtonBox::rejected, this, &InstrumentEditor::close);
}

// -----------------------------------------------------------------------------
InstrumentEditor::~InstrumentEditor() {
  delete mUi;
}

// -----------------------------------------------------------------------------
void InstrumentEditor::setInstrument(const Sft::InstrumentPtr &instrument) {
  mInstrument = nullptr;

  mUi->instrumentPreview->setInstrument(instrument);

  mUi->sampleId->setEditText(instrument->sample()->name());
  mUi->sampleEnd->setMaximum(instrument->sample()->maximumDuration() * 1000);
  mUi->sampleEnd->setValue(instrument->sample()->maximumDuration() * 1000);
  mUi->sampleQuality->setValue(instrument->sample()->compressionFactor() * 100.0);

  if (instrument) {
    mUi->sampleEnd->setEnabled(!instrument->sample()->isRawBrrSample());
    mUi->sampleQuality->setEnabled(!instrument->sample()->isRawBrrSample());
  }

  mUi->instrumentName->setText(instrument->name());
  mUi->instrumentColor->setCurrentColor(RenderHelper::toQColor(instrument->color()));

  mUi->instrumentNoteAdjust->setValue(instrument->noteAdjust());
  mUi->instrumentVolume->setValue(instrument->volume() * 100.0);
  mUi->instrumentLoopStart->setMaximum(instrument->sample()->maximumDuration() * 1000);
  mUi->instrumentLoopStart->setValue(instrument->sample()->loopStart() * 1000);
  mUi->instrumentLoopStartBlock->setMaximum(instrument->sample()->brrSample()->numChunks());
  mUi->instrumentLoopStartBlock->setValue(instrument->sample()->loopStartChunk());
  mUi->instrumentLooped->setChecked(instrument->isLooped());

  mUi->instrumentAttack->setValue(15 - instrument->adsr().a);
  mUi->instrumentDecay->setValue(7 - instrument->adsr().d);
  mUi->instrumentSustain->setValue(7 - instrument->adsr().s);
  mUi->instrumentRelease->setValue(31 - instrument->adsr().r);

  mInstrument = instrument;

  updateSampleSize();
}

// -----------------------------------------------------------------------------
void InstrumentEditor::playPreview() {
  mUi->instrumentPreview->update();
}

// -----------------------------------------------------------------------------
void InstrumentEditor::updateAdsrValue() {
  if (!mInstrument) {
    return;
  }

  mInstrument->setAdsr(Sft::InstrumentAdsr(
    15 - mUi->instrumentAttack->value(),
    7 - mUi->instrumentDecay->value(),
    7 - mUi->instrumentSustain->value(),
    31 - mUi->instrumentRelease->value()
  ));

  playPreview();
}

// -----------------------------------------------------------------------------
void InstrumentEditor::updateVolume() {
  if (!mInstrument) {
    return;
  }

  mInstrument->setVolume(mUi->instrumentVolume->value() / 100.0);
  playPreview();
}

// -----------------------------------------------------------------------------
void InstrumentEditor::updateLoopStart() {
  if (!mInstrument || mPreventUpdate) {
    return;
  }

  mPreventUpdate = true;
  mInstrument->sample()->setLoopStart(mUi->instrumentLoopStart->value() / 1000.0);
  mUi->instrumentLoopStartBlock->setValue(mInstrument->sample()->loopStartChunk());
  mPreventUpdate = false;

  playPreview();
}

// -----------------------------------------------------------------------------
void InstrumentEditor::updateLoopStartBlock() {
  if (!mInstrument || mPreventUpdate) {
    return;
  }
  
  mPreventUpdate = true;
  mInstrument->sample()->setLoopStartChunk(mUi->instrumentLoopStartBlock->value());
  mUi->instrumentLoopStart->setValue(mInstrument->sample()->loopStart() * 1000);
  mPreventUpdate = false;

  playPreview();
}

// -----------------------------------------------------------------------------
void InstrumentEditor::updateLooped() {
  if (!mInstrument) {
    return;
  }

  mInstrument->setLooped(mUi->instrumentLooped->isChecked());
  mInstrument->sample()->setLooped(mUi->instrumentLooped->isChecked());
  playPreview();
}

// -----------------------------------------------------------------------------
void InstrumentEditor::updateNoteAdjust() {
  if (!mInstrument) {
    return;
  }

  mInstrument->setNoteAdjust(mUi->instrumentNoteAdjust->value());
  playPreview();
}

// -----------------------------------------------------------------------------
void InstrumentEditor::updateSampleEnd() {
  if (!mInstrument) {
    return;
  }

}

// -----------------------------------------------------------------------------
void InstrumentEditor::updateQuality() {
  if (!mInstrument) {
    return;
  }

  double factor = qMin(1.0, qMax(0.01, (double)mUi->sampleQuality->value() / 100.0));
  mInstrument->sample()->setCompressionFactor(factor);
  mUi->instrumentPreview->update();

  updateSampleSize();
}

// -----------------------------------------------------------------------------
void InstrumentEditor::updateSampleSize() {
  if (!mInstrument) {
    return;
  }

  int size =mInstrument->sample()->brrSample()->streamLengthInBytes();
  mUi->sampleSizeLabel->setText(QString::number(size));
  mUi->sampleSizePercentLabel->setText(QString::number(size / (double)0x8000 * 100.0) + " %");
}

// -----------------------------------------------------------------------------
void InstrumentEditor::updateName() {
  if (!mInstrument) {
    return;
  }

  mInstrument->setName(mUi->instrumentName->text());
}