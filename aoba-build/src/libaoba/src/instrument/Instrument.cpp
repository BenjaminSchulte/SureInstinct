#include <QRandomGenerator>
#include <QDebug>
#include <aoba/log/Log.hpp>
#include <QFile>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/CalculatedNumber.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/sample/Sample.hpp>
#include <aoba/instrument/Instrument.hpp>
#include <sft/instrument/Instrument.hpp>
#include <sft/sample/Sample.hpp>
#include <sft/brr/BrrSample.hpp>
#include <yaml-cpp/yaml.h>

using namespace Aoba;

// -----------------------------------------------------------------------------
InstrumentAssetSet::InstrumentAssetSet(Project *project, SampleAssetSet *samples, uint8_t instrumentBank)
  : LinkedAssetSet<Instrument>("instrument", project)
  , mSamples(samples)
{
  mInstrumentIndex = mLinkerObject.createLinkerBlock("__asset_instrument_index", instrumentBank);
  mInstrumentData  = mLinkerObject.createLinkerBlock("__asset_instrument_data", instrumentBank);
}

// -----------------------------------------------------------------------------
InstrumentAssetSet::~InstrumentAssetSet() {

}

// -----------------------------------------------------------------------------
Instrument::Instrument(InstrumentAssetSet *set, const QString &id, const QString &dir)
  : Asset(set, id)
  , mFile(dir)
{
  set->add(this);
}

// -----------------------------------------------------------------------------
Instrument::~Instrument() {
  delete mInstrument;
}

// -----------------------------------------------------------------------------
bool Instrument::reload() {
  YAML::Node config;
  try {
    config = YAML::LoadFile(mFile.toStdString().c_str());
  } catch(YAML::BadFile&) {
    Aoba::log::error("Failed to load instrument YML");
    return false;
  }

  mSample = mAssetSet->samples()->get(config["sample"].as<std::string>().c_str());
  if (!mSample) {
    Aoba::log::error("Could not find sample");
    return false;
  }

  try {
    if (!config["loop"].as<bool>()) {
      mLoop = -1;
    } else {
      mLoop = config["loop"].as<bool>() ? 0 : -1;;
    }
  } catch(YAML::TypedBadConversion<bool> &e) {
    try {
      double loop = config["loop"].as<double>() / 1000;
      mLoop = loop / mSample->sample()->maximumDuration() * mSample->sample()->brrSample()->numChunks() * 9;
    } catch(YAML::TypedBadConversion<double> &e) {
      mLoop = -1;
    }
  }
  if ((mLoop / 9) >= (int32_t)mSample->sample()->brrSample()->numChunks()) {
    mLoop = mSample->sample()->brrSample()->numChunks() * 9 - 9;
  }

  if (!mSample->sample()->isRawBrrSample()) {
    if ((mSample->sample()->isLooped() && mLoop < 0)
      || (!mSample->sample()->isLooped() && mLoop >= 0)
      || (mLoop >= 0 && (int32_t)mSample->sample()->loopStartChunk() != (mLoop / 9))
    ) {
      Aoba::log::warn("Sample loop for '" + mSample->id() + "' has different loop configuration than instrument '" + mId + "'");
    }
  }

  mInstrument = new Sft::Instrument(mSample->sample(), mId, mId);
  mInstrument->setVolume(config["volume"].as<int>());
  mPitch = config["pitch"].as<int>();
  mUseAdsr = config["adsr"]["enabled"].as<bool>();

  mInstrument->setAdsr(Sft::InstrumentAdsr(
    config["adsr"]["ar"].as<int>(),
    config["adsr"]["dr"].as<int>(),
    config["adsr"]["sl"].as<int>(),
    config["adsr"]["sr"].as<int>()
  ));

  return true;
}

// -----------------------------------------------------------------------------
bool Instrument::build() {
  uint16_t dataSize = (mSample->sizeInBytes() + 2) / 3;

  if (mLoop >= mSample->sizeInBytes()) {
    Aoba::log::warn("Instrument " + mId + " loop is behind end of sample");
    mLoop = mSample->sizeInBytes() - 9;
  }

  FMA::linker::LinkerBlock *data = mAssetSet->instrumentData();
  mAssetSet->instrumentIndex()->write(FMA::symbol::ReferencePtr(
    new FMA::symbol::CalculatedNumber(
      FMA::symbol::ReferencePtr(new FMA::symbol::SymbolReference("__asset_instrument_data")),
      FMA::symbol::CalculatedNumber::ADD,
      FMA::symbol::ReferencePtr(new FMA::symbol::ConstantNumber(data->getSize()))
    )
  ), 2);

  uint16_t adsr = (
    mUseAdsr ? (
      0x0080 |
      (mInstrument->adsr().a) |
      (mInstrument->adsr().d << 4) |
      (mInstrument->adsr().s << 13) |
      (mInstrument->adsr().r << 8)
    ) : 0
  );

  if (mLoop < 0) {
    mLoop = 0x8000 | QRandomGenerator::global()->bounded(0x8000);
  } else if (mLoop >= 0x8000) {
    Aoba::log::error("Loop offset if out of range");
    return false;
  }

  int pitchAdjust = mInstrument->sample()->playbackNoteAdjust() * 0x100;
      
  data->writeNumber(mLoop, 2);
  data->writeNumber(mInstrument->volume(), 1);
  data->writeNumber(mPitch + pitchAdjust, 2);
  data->writeNumber(adsr, 2);
  data->writeNumber(dataSize, 2);
  data->write(FMA::symbol::ReferencePtr(new FMA::symbol::SymbolReference(mSample->symbolName().toStdString())), 3);

  return true;
}

// -----------------------------------------------------------------------------
QStringList Instrument::getFmaObjectFiles() const {
  return mAssetSet->assetLinkerObject().getFmaObjectFiles();
}

// -----------------------------------------------------------------------------
QString Instrument::getFmaCode() const {
  return "";
}
