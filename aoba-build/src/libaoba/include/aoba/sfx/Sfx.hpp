#pragma once

#include <QString>
#include <QSharedPointer>
#include "../song/AobaSongBuilderAdapter.hpp"
#include "../asset/Asset.hpp"
#include "../asset/LinkedAssetSet.hpp"

namespace Aoba {
class Sfx;
class SongBuilder;
class InstrumentAssetSet;

class SfxAssetSet : public LinkedAssetSet<Sfx> {
public:
  //! Constructor
  SfxAssetSet(Project *project, InstrumentAssetSet *instruments);

  //! Deconstructor
  ~SfxAssetSet();

  //! Returns the instruments
  inline InstrumentAssetSet *instruments() const { return mInstruments; }

private:
  //! Instruments
  InstrumentAssetSet *mInstruments;
};

class Sfx : public Asset<SfxAssetSet> {
public:
  //! Constructor
  Sfx(SfxAssetSet *set, const QString &id, const QString &file);

  //! Deconstructor
  ~Sfx();

  //! Loads the asset
  bool reload() override;

  //! Builds the asset
  bool build() override;

  //! Links the sfxs
  QStringList getFmaObjectFiles() const override;

  //! Returns the song builder
  inline SongBuilder *songBuilder() const { return mSongBuilder; }

  //! Returns the necessary FMA code
  QString getFmaCode() const override;

  //! Returns the file
  inline const QString &file() const { return mFile; }

protected:
  //! The adapter
  AobaSongBuilderAdapter mSongBuilderAdapter;

  //! The song builder
  SongBuilder *mSongBuilder;

  //! The containing file
  QString mFile;
};

}