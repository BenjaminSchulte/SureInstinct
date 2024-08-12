#pragma once

#include <QMap>
#include "BaseImage.hpp"
#include "ImageAccessor.hpp"

namespace Aoba {
class RawImageAssetSet;

class RawImage : public BaseImage {
public:
  //! Constructor
  RawImage(RawImageAssetSet *set, const QString &id, const QDir &path);

  //! Deconstructor
  ~RawImage();

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
  RawImageAssetSet *imageAssetSet() const;

private:
  //! Adds a variant
  bool addVariant(const QString &imageFileName, const QString &paletteFileName);

  //! The containing path
  QDir mPath;

  //! The result image list
  ImageList mImages;

  //! Generate order
  bool mRowsBeforeColumns;
};

}