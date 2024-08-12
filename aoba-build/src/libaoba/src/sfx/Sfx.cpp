#include <aoba/log/Log.hpp>
#include <QFile>
#include <sft/song/Song.hpp>
#include <sft/mml/MmlParser.hpp>
#include <fma/linker/LinkerBlock.hpp>
#include <fma/symbol/SymbolReference.hpp>
#include <aoba/project/Project.hpp>
#include <aoba/sfx/Sfx.hpp>
#include <aoba/song/SongBuilder.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
SfxAssetSet::SfxAssetSet(Project *project, InstrumentAssetSet *instruments)
  : LinkedAssetSet<Sfx>("sfx", project)
  , mInstruments(instruments)
{
  //mSfxIndex = assetLinkerObject().createLinkerBlock("__sfx_index");
}

// -----------------------------------------------------------------------------
SfxAssetSet::~SfxAssetSet() {
}


// -----------------------------------------------------------------------------
Sfx::Sfx(SfxAssetSet *set, const QString &id, const QString &dir)
  : Asset(set, id)
  , mSongBuilderAdapter(set->instruments())
  , mSongBuilder(new SongBuilder(&mSongBuilderAdapter))
  , mFile(dir)
{
  set->add(this);
}

// -----------------------------------------------------------------------------
Sfx::~Sfx() {
  delete mSongBuilder;
}

// -----------------------------------------------------------------------------
bool Sfx::reload() {
  Sft::MmlParser parser(mSongBuilder);
  if (!parser.load(mFile)) {
    Aoba::log::error("Failed to load MML sfx " + mFile);
    return false;
  }

  return !mSongBuilder->didFail();
}

// -----------------------------------------------------------------------------
bool Sfx::build() {
  return true;
  //
  //FMA::linker::LinkerBlock *block = mAssetSet->assetLinkerObject().createLinkerBlock(symbolName());
  //mSongBuilder->writeTo(block);
  //mAssetSet->sfxIndex()->write(FMA::symbol::ReferencePtr(new FMA::symbol::SymbolReference(symbolName().toStdString())), 3);
  //return true;
}

// -----------------------------------------------------------------------------
QStringList Sfx::getFmaObjectFiles() const {
  return mAssetSet->assetLinkerObject().getFmaObjectFiles();
}

// -----------------------------------------------------------------------------
QString Sfx::getFmaCode() const {
  return "";
}
