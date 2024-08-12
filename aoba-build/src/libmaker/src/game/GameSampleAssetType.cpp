#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameSampleAssetType.hpp>

using namespace Maker;
using namespace Aoba;


// -----------------------------------------------------------------------------
Aoba::Sample *GameSampleAsset::configure(const GameConfigNode &config) {
  Aoba::SampleAssetSet *samples = dynamic_cast<Aoba::SampleAssetSet*>(mType->assetSet());

  return new Aoba::Sample(samples, mId, config.asAssetFileName("samples", "sample.brr"));
}


// -----------------------------------------------------------------------------
SampleAssetSet *GameSampleAssetType::configure(const GameConfigNode &config) {
  SampleAssetSet *set = new SampleAssetSet(mProject->project());

  for (const auto &option : config["stream_bytes_per_frame"].asList()) {
    if (!option.isInt()) {
      continue;
    }

    set->addStreamBytesPerFrameOption(option.asInt());
  }

  return set;
}

// -----------------------------------------------------------------------------
GameSampleAsset *GameSampleAssetType::create(const QString &id) const {
  return new GameSampleAsset(this, mProject, id);
}

// -----------------------------------------------------------------------------
