#pragma once

#include <QString>
#include <QSharedPointer>
#include "../asset/Asset.hpp"
#include "../asset/LinkedAssetSet.hpp"
#include <sft/instrument/Instrument.hpp>

namespace Aoba {
class Stream;
class Sample;
class SampleAssetSet;

class StreamAssetSet : public LinkedAssetSet<Stream> {
public:
  //! Constructor
  StreamAssetSet(Project *project, SampleAssetSet *samples, uint8_t streamBank);

  //! Deconstructor
  ~StreamAssetSet();

  //! Returns the samples
  inline SampleAssetSet *samples() const { return mSamples; }

  //! Returns the stream header bank
  inline uint8_t streamHeaderBank() const { return mStreamHeaderBank; }

private:
  //! Samples
  SampleAssetSet *mSamples;

  //! The stream header bank
  uint8_t mStreamHeaderBank;
};

class Stream : public Asset<StreamAssetSet> {
public:
  //! Constructor
  Stream(StreamAssetSet *set, const QString &id, const QString &file);

  //! Deconstructor
  ~Stream();

  //! Returns the symbol name
  QString symbolName() const;

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

  //! The ID of the sample
  Sample *mSample;

  //! Instrument
  Sft::Instrument *mInstrument;
};

}