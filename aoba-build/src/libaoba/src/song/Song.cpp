#include <aoba/log/Log.hpp>
#include <QFile>
#include <sft/song/Song.hpp>
#include <sft/mml/MmlParser.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/output/DynamicBuffer.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/song/Song.hpp>
#include <aoba/song/SongBuilder.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
SongAssetSet::SongAssetSet(Project *project, InstrumentAssetSet *instruments)
  : LinkedAssetSet<Song>("song", project)
  , mInstruments(instruments)
{
  mSongIndex = assetLinkerObject().createLinkerBlock("__song_index");
}

// -----------------------------------------------------------------------------
SongAssetSet::~SongAssetSet() {
}


// -----------------------------------------------------------------------------
Song::Song(SongAssetSet *set, const QString &id, const QString &dir)
  : Asset(set, id)
  , mSongBuilderAdapter(set->instruments())
  , mSongBuilder(new SongBuilder(&mSongBuilderAdapter))
  , mFile(dir)
{
  set->add(this);
}

// -----------------------------------------------------------------------------
Song::~Song() {
  delete mSongBuilder;
}

// -----------------------------------------------------------------------------
QString Song::symbolName() const {
  return "__asset_song_" + mId;
}

// -----------------------------------------------------------------------------
bool Song::reload() {
  Sft::MmlParser parser(mSongBuilder);
  if (!parser.load(mFile)) {
    Aoba::log::error("Failed to load MML song " + mFile);
    return false;
  }

  return !mSongBuilder->didFail();
}

// -----------------------------------------------------------------------------
bool Song::build() {
  FMA::output::DynamicBuffer buffer;
  mSongBuilder->writeTo(&buffer);

  FMA::linker::LinkerBlock *block = mAssetSet->assetLinkerObject().createLinkerBlock(symbolName());
  block->write(buffer.getData(), buffer.getSize());
  mAssetSet->songIndex()->write(FMA::symbol::ReferencePtr(new FMA::symbol::SymbolReference(symbolName().toStdString())), 3);
  return true;
}

// -----------------------------------------------------------------------------
QStringList Song::getFmaObjectFiles() const {
  return mAssetSet->assetLinkerObject().getFmaObjectFiles();
}

// -----------------------------------------------------------------------------
QString Song::getFmaCode() const {
  return "";
}
