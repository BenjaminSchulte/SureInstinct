#pragma once

#include <QSharedPointer>
#include "../types/Color.hpp"

namespace Sft {
class Sample;

typedef QSharedPointer<class Instrument> InstrumentPtr;
typedef QWeakPointer<class Instrument> WeakInstrumentPtr;

struct InstrumentAdsr {
  //! Constructor
  InstrumentAdsr() : a(0x0F), d(0x07), s(0x07), r(0x0) {}

  //! Constructor
  InstrumentAdsr(uint8_t a, uint8_t d, uint8_t s, uint8_t r) : a(a), d(d), s(s), r(r) {}

  //! Returns the Attack in seconds
  double attackDuration() const;

  //! Returns the Decay in seconds
  double decayDuration() const;

  //! Returns the sustain in percent
  double sustainPercent() const;

  //! Returns the release in seconds
  double releaseDuration() const;

  //! Attack (0..15)
  uint8_t a;

  //! Decay (0..7)
  uint8_t d;

  //! Sustain (0..7)
  uint8_t s;

  //! Release (0..31)
  uint8_t r;
};

class Instrument : public QObject {
  Q_OBJECT

public:
  //! Constructor
  Instrument(Sample *sample, const QString &id, const QString &name);

  //! Deconstructor
  ~Instrument();

  //! Sets the ID
  void setId(const QString &id) { mId = id; }

  //! Sets the ID
  void setName(const QString &name) { mName = name; emit nameChanged(); }

  //! Sets the ID
  void setColor(const Color &color) { mColor = color; emit colorChanged(); }

  //! Returns the sample
  inline Sample *sample() const { return mSample; }

  //! Returns the id
  inline const QString &id() const { return mId; }

  //! Returns the name
  inline const QString &name() const { return mName; }

  //! Returns the color
  inline const Color &color() const { return mColor; }

  //! Returns the total note adjust
  double playbackNoteAdjust() const;

  //! Returns the note adjustment
  inline double noteAdjust() const { return mNoteAdjust; }

  //! Returns the note adjustment
  inline void setNoteAdjust(double noteAdjust) { mNoteAdjust = noteAdjust; }

  //! Returns the instrument volume
  inline double volume() const { return mVolume; }

  //! Sets the volume
  inline void setVolume(double volume) { mVolume = volume; }

  //! Returns the loop
  inline bool isLooped() const { return mLooped; }

  //! Sets the looped
  inline void setLooped(bool looped) { mLooped = looped; }

  //! Returns the envelope
  inline const InstrumentAdsr &adsr() const { return mADSR; }

  //! Returns the envelope
  inline void setAdsr(const InstrumentAdsr &adsr) { mADSR = adsr; }

signals:
  //! The name has been modified
  void nameChanged();

  //! The color has been modified
  void colorChanged();

protected:
  //! The sample
  Sample *mSample;

  //! The id
  QString mId;

  //! The name
  QString mName;

  //! The color of the instrument
  Color mColor;

  //! The note adjust (1.0 = 1 note, 0.5 = half note)
  double mNoteAdjust = 0;

  //! The volume
  double mVolume = 1.0;

  //! Whether the instrument is looped
  bool mLooped = false;

  //! The envelope
  InstrumentAdsr mADSR;
};

}