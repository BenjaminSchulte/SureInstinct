#include <aoba/log/Log.hpp>
#include <maker/game/GameConfigReader.hpp>
#include <maker/game/GameProject.hpp>
#include <maker/game/GameSongAssetType.hpp>
#include <maker/game/GameInstrumentAssetType.hpp>
#include <aoba/song/SongScriptAssetResolver.hpp>

using namespace Maker;
using namespace Aoba;


// -----------------------------------------------------------------------------
Aoba::Song *GameSongAsset::configure(const GameConfigNode &config) {
  Aoba::SongAssetSet *instruments = dynamic_cast<Aoba::SongAssetSet*>(mType->assetSet());

  return new Aoba::Song(instruments, mId, config.asAssetFileName("songs", "song.mml"));
}


// -----------------------------------------------------------------------------
SongAssetSet *GameSongAssetType::configure(const GameConfigNode &config) {
  QString instrumentId = config["instruments"].asString("");
  GameInstrumentAssetType *instruments = mProject->assetTypes().get<GameInstrumentAssetType>(instrumentId);
  if (!instruments) {
    mProject->log().error("Could not find instrument type " + instrumentId);
    return nullptr;
  }

  InstrumentAssetSet *instrumentSet = dynamic_cast<InstrumentAssetSet*>(instruments->assetSet());
  SongAssetSet *set = new SongAssetSet(mProject->project(), instrumentSet);
  return set;
}

// -----------------------------------------------------------------------------
Aoba::ScriptAssetResolver *GameSongAssetType::createAssetResolver() const {
  return new SongScriptAssetResolver(dynamic_cast<SongAssetSet*>(mAssetSet));
}

// -----------------------------------------------------------------------------
GameSongAsset *GameSongAssetType::create(const QString &id) const {
  return new GameSongAsset(this, mProject, id);
}

// -----------------------------------------------------------------------------
