#include <aoba/log/Log.hpp>
#include <QFile>
#include <fma/linker/LinkerBlock.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/sample/Sample.hpp>
#include <sft/brr/BrrSample.hpp>
#include <sft/sample/Sample.hpp>
#include <sft/sample/SampleData.hpp>
#include <sft/sample/BrrSampleLoader.hpp>
#include <sft/sample/WavSampleLoader.hpp>
#include <yaml-cpp/yaml.h>
#include <aoba/yaml/YamlTools.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
Sample::Sample(SampleAssetSet *set, const QString &id, const QString &dir)
  : Asset(set, id)
  , mFile(dir)
  , mSample(nullptr)
{
  set->add(this);
}

// -----------------------------------------------------------------------------
Sample::~Sample() {
  delete mSample;
}

// -----------------------------------------------------------------------------
bool Sample::reload() {
  mSample = new Sft::Sample(mId, mId);
  
  if (mFile.endsWith(".brr")) {
    Sft::BrrSampleLoader loader(mSample);
    if (!loader.load(mFile)) {
      Aoba::log::error("Failed to load BRR sample " + mFile);
      delete mSample;
      return false;
    }
  } else if (mFile.endsWith(".yml")) {
    return reloadFromYml();
  } else {
    Aoba::log::error("Unsupported sample file format " + mFile);
    return false;
  }

  return true;
}

// -----------------------------------------------------------------------------
bool Sample::reloadFromYml() {
  QString waveFile = mFile;
  waveFile.replace(".yml", ".wav");

  Sft::WavSampleLoader loader(mSample);
  if (!loader.load(waveFile)) {
    Aoba::log::error("Failed to load WAV sample: " + waveFile);
    delete mSample;
    return false;
  }

  YAML::Node config;
  try {
    config = YAML::LoadFile(mFile.toStdString().c_str());
  } catch(YAML::BadFile&) {
    return false;
  }

  double quality = Yaml::asDouble(config["quality"], 1);
  mStreamable = Yaml::asBool(config["streamable"]); 

  double loop = -1;
  if (!mStreamable) {
    try {
      if (!config["loop"].as<bool>()) {
        loop = -1;
      } else {
        loop = config["loop"].as<bool>() ? 0 : -1;
      }
    } catch(YAML::TypedBadConversion<bool> &e) {
      try {
        loop = config["loop"].as<double>() / 1000.0;
      } catch(YAML::TypedBadConversion<double> &e) {
        loop = -1;
      }
    }
  }

  mSample->setLooped(loop >= 0);
  mSample->setLoopStart(loop < 0 ? 0 : loop);

  if (mStreamable) {
    int fps = mAssetSet->project()->framesPerSecond();
    double inputSampleRate = mSample->inputData()->sampleRate();
    int bytesPerFrame = qRound(inputSampleRate / 16 * 9 * quality / fps);
    mBytesPerFrame = findClosestBytesPerFrame(bytesPerFrame);
    double newQuality = quality / (double)bytesPerFrame * (double)mBytesPerFrame;
    mSample->setCompressionFactor(newQuality);
  } else {
    mSample->setCompressionFactor(quality);
  }

  return true;
}

// -----------------------------------------------------------------------------
QString Sample::symbolName() const {
  return "__asset_sample_" + mId;
}

// -----------------------------------------------------------------------------
uint16_t Sample::sizeInBytes() const {
  return mSample->brrSample()->streamLengthInBytes();
}

// -----------------------------------------------------------------------------
uint16_t Sample::numBrrChunks() const {
  return mSample->brrSample()->numChunks();
}

// -----------------------------------------------------------------------------
bool Sample::build() {
  int size = mSample->brrSample()->streamLengthInBytes();
  uint8_t *data = new uint8_t[size];
  if (!data) {
    return false;
  }

  mSample->brrSample()->writeTo(data);
  FMA::linker::LinkerBlock *block = mAssetSet->assetLinkerObject().createLinkerBlock(symbolName());
  block->write(data, size);

  delete[] data;

  return true;
}

// -----------------------------------------------------------------------------
QStringList Sample::getFmaObjectFiles() const {
  return mAssetSet->assetLinkerObject().getFmaObjectFiles();
}

// -----------------------------------------------------------------------------
QString Sample::getFmaCode() const {
  return "";
}

// -----------------------------------------------------------------------------
int Sample::findClosestBytesPerFrame(int oldBytesPerFrame) const {
  uint32_t nearestDiff = 0xFFFFFFFF;
  int nearestBytesPerFrame = oldBytesPerFrame;

  for (uint32_t option : mAssetSet->streamBytesPerFrame()) {
    uint32_t diff = qAbs((int32_t)oldBytesPerFrame - (int32_t)option);

    if (diff < nearestDiff) {
      nearestDiff = diff;
      nearestBytesPerFrame = option;
    }
  }

  return nearestBytesPerFrame;
}