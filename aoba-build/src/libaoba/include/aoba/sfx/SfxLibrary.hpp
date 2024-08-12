#pragma once

#include <QString>
#include <QSharedPointer>
#include "../asset/Asset.hpp"
#include "../asset/LinkedAssetSet.hpp"

namespace Aoba {
class Sfx;
class SfxLibrary;
class SfxAssetSet;

class SfxLibraryAssetSet : public LinkedAssetSet<SfxLibrary> {
public:
  //! Constructor
  SfxLibraryAssetSet(Project *project, SfxAssetSet *sfx);

  //! Deconstructor
  ~SfxLibraryAssetSet();

  //! Returns the SFX database
  inline SfxAssetSet *sfxDatabase() const { return mSfxDatabase; }

private:
  //! The SFX database
  SfxAssetSet *mSfxDatabase;
};

class SfxLibrary : public Asset<SfxLibraryAssetSet> {
public:
  //! Constructor
  SfxLibrary(SfxLibraryAssetSet *set, const QString &id, const QString &file);

  //! Deconstructor
  ~SfxLibrary();

  //! Returns the symbol name
  QString symbolName() const;

  //! Loads the asset
  bool reload() override;

  //! Builds the asset
  bool build() override;

  //! Returns the parent SFX list
  SfxLibrary *getParentSfxLibrary() const;

  //! Returns the ID of an SFX
  int sfxId(const QString &sfx) const;

  //! Returns the ID of an SFX
  int sfxCount() const;

  //! Returns a list of all SFX items
  QVector<QString> sfxList() const;

  //! Returns the SFX pointer list
  QVector<Sfx*> sfxAssetList(bool &success) const;

  //! Links the sfxs
  QStringList getFmaObjectFiles() const override;

  //! Returns the necessary FMA code
  QString getFmaCode() const override;

private:
  //! Returns the instrument list with all sfx instruments
  static QVector<int> sfxInstrumentList(const QVector<Sfx*> &sfxList);

  //! The file
  QString mFile;

  //! The parent SFX list
  QString mParentSfxListId;

  //! List of all SFXs
  QVector<QString> mSfxList;
};

}