#pragma once

#include <QVariant>
#include "../asset/Asset.hpp"

namespace Aoba {

class PaletteAssetSet;
class PaletteGroup;
class PaletteGroupComponent;
class Palette;

class PaletteGroupSet : public AssetSet<PaletteGroup> {
public:
  PaletteGroupSet(Project *project, const QString &name, PaletteAssetSet *set, int paletteBase)
    : AssetSet<PaletteGroup>(name, project)
    , mPaletteAssetSet(set)
    , mPaletteBase(paletteBase)
  {}

  //! Returns the palette set
  inline PaletteAssetSet *paletteSet() const { return mPaletteAssetSet; }

  //! Adds a component
  void addComponent(PaletteGroupComponent *comp) {
    mComponents.push_back(comp);
  }

  //! Returns all components
  inline const QVector<PaletteGroupComponent*> components() const { return mComponents; }

  //! Returns the base palette
  inline int paletteBase() const { return mPaletteBase; }

private:
  //! The palette set
  PaletteAssetSet *mPaletteAssetSet;

  //! List of all components
  QVector<PaletteGroupComponent*> mComponents;

  //! Palette base
  int mPaletteBase;
};

class PaletteGroup : public Asset<PaletteGroupSet> {
public:
  //! Constructor
  PaletteGroup(PaletteGroupSet *set, const QString &id);

  //! Deconstructor
  ~PaletteGroup();

  //! Inserts a palette
  Palette *add(int index, Palette *p);

  //! Inserts a palette
  Palette *add(int index, const QString &file, int row=-1);

  //! Returns the symbol name
  QString assetSymbolName() const;

  //! Loads the asset
  bool reload() override;

  //! Builds the asset
  bool build() override;

  //! Returns the necessary FMA code
  QString getFmaCode() const override;

  //! Returns all properties
  inline QMap<QString, QVariant> &properties() { return mProperties; }

  //! Returns a palette
  inline Palette *get(int index) const { return mPalettes[index]; }

private:
  //! Asset ID
  int mAssetSymbolId;

  //! Map containing all palettes
  QMap<int, Palette*> mPalettes;

  //! List of generated palettes
  QVector<Palette*> mGeneratedPalettes;

  //! The maximum record ID
  int mMaximumRecord = -1;

  //! Properties
  QMap<QString, QVariant> mProperties;
};

}