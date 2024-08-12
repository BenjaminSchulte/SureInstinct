#pragma once

#include <QDir>
#include "../asset/Asset.hpp"
#include "../asset/LinkedAssetSet.hpp"

namespace Aoba {
class BaseImage;
class TilemapAssetSet;
class TilesetAssetSet;

class BaseImageAssetSet : public LinkedAssetSet<BaseImage> {
public:
  //! Constructor
  BaseImageAssetSet(const char *id, Project *project, uint8_t headerRomBank);

  //! Returns the header rom bank
  inline uint8_t headerRomBank() const { return mHeaderRomBank; }

  //! Returns a list of all variants
  virtual QStringList variants() const = 0;

protected:
  //! The header rom bank
  uint8_t mHeaderRomBank;
};

}