#pragma once

#include <QWidget>
#include <QSharedPointer>

class QImage;

namespace Sft {
class Instrument;
typedef QSharedPointer<Instrument> InstrumentPtr;
class Sample;
class SampleData;
}

namespace Editor {

class InstrumentWaveform : public QWidget {
public:
  //! Constructor
  InstrumentWaveform(QWidget *parent = nullptr);

  //! Deconstructor
  ~InstrumentWaveform();

  //! Sets the instrument
  void setInstrument(const Sft::InstrumentPtr &inst);

  //! Sets the sample
  void setSample(Sft::Sample *sample);

protected:
  //! Returns the center position
  inline int center() const { return size().height() / 2; }

  //! Returns the wave height
  inline int waveHeight() const { return center() - 10; }

  //! Render event
  void paintEvent(QPaintEvent *) override;

  //! Resize event
  void resizeEvent(QResizeEvent *) override;

  //! Deletes all cached images
  void deleteCachedImages();

  //! Regenerates the cache
  void regenerateCache();

  //! Draws a waveform
  void drawWaveform(QImage *target, const Sft::SampleData *data, const QColor &color);

  //! The sample
  Sft::Sample *mSample = nullptr;

  //! The instrument
  Sft::InstrumentPtr mInstrument;

  //! Whether the cache is valid
  bool mCacheIsValid = false;

  //! Image for sample
  QImage *mOriginalSampleWaveform = nullptr;
};

}