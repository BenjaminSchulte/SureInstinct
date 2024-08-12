#pragma once

#include <QString>
#include <QSharedPointer>
#include "../song/AobaSongBuilderAdapter.hpp"
#include "../asset/Asset.hpp"
#include "../asset/LinkedAssetSet.hpp"

namespace Aoba {
class Song;
class SongBuilder;
class InstrumentAssetSet;

class SongAssetSet : public LinkedAssetSet<Song> {
public:
  //! Constructor
  SongAssetSet(Project *project, InstrumentAssetSet *instruments);

  //! Deconstructor
  ~SongAssetSet();

  //! Returns the instruments
  inline InstrumentAssetSet *instruments() const { return mInstruments; }

  //! Returns the song index
  inline FMA::linker::LinkerBlock *songIndex() const { return mSongIndex; }

private:
  //! Instruments
  InstrumentAssetSet *mInstruments;

  //! The song index
  FMA::linker::LinkerBlock *mSongIndex;
};

class Song : public Asset<SongAssetSet> {
public:
  //! Constructor
  Song(SongAssetSet *set, const QString &id, const QString &file);

  //! Deconstructor
  ~Song();

  //! Returns the symbol name
  QString symbolName() const;

  //! Loads the asset
  bool reload() override;

  //! Builds the asset
  bool build() override;

  //! Links the songs
  QStringList getFmaObjectFiles() const override;

  //! Returns the necessary FMA code
  QString getFmaCode() const override;

  //! Returns the file
  inline const QString &file() const { return mFile; }

protected:
  //! The song builder adapter
  AobaSongBuilderAdapter mSongBuilderAdapter;

  //! The song builder
  SongBuilder *mSongBuilder;

  //! The containing file
  QString mFile;
};

}