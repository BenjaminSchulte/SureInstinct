#pragma once

#include <QString>
#include <QSharedPointer>
#include "../asset/Asset.hpp"
#include "../asset/LinkedAssetSet.hpp"
#include <sft/instrument/Instrument.hpp>

namespace Aoba {
class Instrument;
class Sample;
class SampleAssetSet;

class InstrumentAssetSet : public LinkedAssetSet<Instrument> {
public:
  //! Constructor
  InstrumentAssetSet(Project *project, SampleAssetSet *samples, uint8_t instrumentBank);

  //! Deconstructor
  ~InstrumentAssetSet();

  //! Returns the samples
  inline SampleAssetSet *samples() const { return mSamples; }

  //! Returns the instrument index
  inline FMA::linker::LinkerBlock *instrumentIndex() const { return mInstrumentIndex; }

  //! Returns the instrument data
  inline FMA::linker::LinkerBlock *instrumentData() const { return mInstrumentData; }

private:
  //! Samples
  SampleAssetSet *mSamples;

  //! The instrument bank

  //! Linker block for all instruments
  FMA::linker::LinkerBlock *mInstrumentIndex;

  //! Linker block for all instruments
  FMA::linker::LinkerBlock *mInstrumentData;
};

class Instrument : public Asset<InstrumentAssetSet> {
public:
  //! Constructor
  Instrument(InstrumentAssetSet *set, const QString &id, const QString &file);

  //! Deconstructor
  ~Instrument();

  //! Loads the asset
  bool reload() override;

  //! Builds the asset
  bool build() override;

  //! REturns the FMA objects
  QStringList getFmaObjectFiles() const override;

  //! Returns the necessary FMA code
  QString getFmaCode() const override;

  //! Returns the file
  inline const QString &file() const { return mFile; }

protected:
  //! The containing file
  QString mFile;

  //! Whether ADSR should be used
  bool mUseAdsr = false;

  //! The ID of the sample
  Sample *mSample;

  //! The pitch
  int mPitch = 0;

  //! Loop position (-1 = no loop)
  int mLoop = -1;

  //! The ADSR
  Sft::Instrument *mInstrument = nullptr;
};

}