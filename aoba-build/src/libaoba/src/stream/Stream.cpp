#include <aoba/log/Log.hpp>
#include <QFile>
#include <cmath>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/CalculatedNumber.hpp>
#include <fma/symbol/ConstantNumber.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/sample/Sample.hpp>
#include <aoba/stream/Stream.hpp>
#include <sft/instrument/Instrument.hpp>
#include <yaml-cpp/yaml.h>

using namespace Aoba;

// -----------------------------------------------------------------------------
StreamAssetSet::StreamAssetSet(Project *project, SampleAssetSet *samples, uint8_t streamBank)
  : LinkedAssetSet<Stream>("stream", project)
  , mSamples(samples)
  , mStreamHeaderBank(streamBank)
{
}

// -----------------------------------------------------------------------------
StreamAssetSet::~StreamAssetSet() {

}

// -----------------------------------------------------------------------------
Stream::Stream(StreamAssetSet *set, const QString &id, const QString &dir)
  : Asset(set, id)
  , mFile(dir)
{
  set->add(this);
}

// -----------------------------------------------------------------------------
Stream::~Stream() {
  delete mInstrument;
}

// -----------------------------------------------------------------------------
QString Stream::symbolName() const {
  return "__asset_stream_" + mId;
}

// -----------------------------------------------------------------------------
bool Stream::reload() {
  YAML::Node config;
  try {
    config = YAML::LoadFile(mFile.toStdString().c_str());
  } catch(YAML::BadFile&) {
    Aoba::log::error("Failed to load stream YML");
    return false;
  }

  mSample = mAssetSet->samples()->get(config["sample"].as<std::string>().c_str());
  if (!mSample) {
    Aoba::log::error("Could not find sample");
    return false;
  }

  if (!mSample->streamable()) {
    Aoba::log::error("Using sample " + mSample->id() + " as stream which is not marked as streamable");
    return false;
  }

  mInstrument = new Sft::Instrument(mSample->sample(), mId, mId);
  mInstrument->setVolume(config["volume"].as<int>());

  return true;
}

// -----------------------------------------------------------------------------
bool Stream::build() {
  FMA::linker::LinkerBlock *block = mAssetSet->assetLinkerObject().createLinkerBlock(symbolName(), assetSet()->streamHeaderBank());

  int fps = mAssetSet->project()->framesPerSecond();
  double hz = (double)mSample->bytesPerFrame() * fps / 9.0 * 16.0;
  uint16_t pitch = (hz / 32000.0) * 4096.0;
  uint8_t blocksPerFrame = std::ceil((double)mSample->bytesPerFrame() / 9.0 / 4.0);

  block->writeNumber(blocksPerFrame * 2, 1);
  block->writeNumber(pitch, 2);
  block->writeNumber(std::ceil((double)mSample->sizeInBytes() / mSample->bytesPerFrame()), 2);
  block->write(FMA::symbol::ReferencePtr(new FMA::symbol::SymbolReference(mSample->symbolName().toStdString())), 3);

  return true;
}

// -----------------------------------------------------------------------------
QStringList Stream::getFmaObjectFiles() const {
  return mAssetSet->assetLinkerObject().getFmaObjectFiles();
}

// -----------------------------------------------------------------------------
QString Stream::getFmaCode() const {
  return "";
}
