#pragma once

#include <QMap>
#include "BaseImage.hpp"

namespace Aoba {

class Image : public BaseImage {
public:
  //! Constructor
  Image(ImageAssetSet *set, const QString &id, const QDir &path);

  //! Deconstructor
  ~Image();

  //! Returns the asset name
  QString assetSymbolName() const override;

  //! Loads the asset
  bool reload() override;

  //! Builds the asset
  bool build() override;

  //! Returns the necessary FMA code
  QString getFmaCode() const override;

  //! Returns additional object files to include
  QStringList getFmaObjectFiles() const override;

  //! Returns the asset set
  ImageAssetSet *imageAssetSet() const;

  //! Returns a list of all variants
  QStringList allVariants() const;

  // Returns the index of an variant  
  int variantIndex(const QString &id) const override { return allVariants().indexOf(id); }

private:
  //! Adds a variant
  bool addVariant(const QString &id, const QString &imageFileName, const QString &paletteFileName, const QString &priorityFileName);

  //! The containing path
  QDir mPath;

  //! The tileset
  Tileset *mTileset = nullptr;

  //! List of all tilemaps per variant
  QMap<QString, Tilemap*> mTilemaps;

  //! List of custom variants
  QStringList mCustomVariants;
};

}