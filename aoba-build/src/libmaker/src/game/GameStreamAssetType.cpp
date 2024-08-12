#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameStreamAssetType.hpp>
#include <maker/game/GameSampleAssetType.hpp>
#include <aoba/stream/StreamScriptAssetResolver.hpp>

using namespace Maker;
using namespace Aoba;


// -----------------------------------------------------------------------------
Aoba::Stream *GameStreamAsset::configure(const GameConfigNode &config) {
  Aoba::StreamAssetSet *streams = dynamic_cast<Aoba::StreamAssetSet*>(mType->assetSet());

  return new Aoba::Stream(streams, mId, config.asAssetFileName("streams", "stream.yml"));
}


// -----------------------------------------------------------------------------
StreamAssetSet *GameStreamAssetType::configure(const GameConfigNode &config) {
  QString sampleId = config["samples"].asString("");
  GameSampleAssetType *samples = mProject->assetTypes().get<GameSampleAssetType>(sampleId);
  if (!samples) {
    mProject->log().error("Could not find sample type " + sampleId);
    return nullptr;
  }

  uint8_t bank = config["stream_bank"].asInt(0xC0);

  SampleAssetSet *sampleSet = dynamic_cast<SampleAssetSet*>(samples->assetSet());
  StreamAssetSet *set = new StreamAssetSet(mProject->project(), sampleSet, bank);
  return set;
}

// -----------------------------------------------------------------------------
GameStreamAsset *GameStreamAssetType::create(const QString &id) const {
  return new GameStreamAsset(this, mProject, id);
}

// -----------------------------------------------------------------------------
Aoba::ScriptAssetResolver *GameStreamAssetType::createAssetResolver() const {
  return new StreamScriptAssetResolver(dynamic_cast<StreamAssetSet*>(mAssetSet));
}

// -----------------------------------------------------------------------------
