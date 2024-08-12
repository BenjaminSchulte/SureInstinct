#pragma once

#include "../script/ScriptAssetResolver.hpp"

namespace Aoba {
class SongAssetSet;

class SongScriptAssetResolver : public ScriptAssetResolver {
public:
  //! Constructor
  SongScriptAssetResolver(SongAssetSet *type) : mType(type) {}

  //! Returns a reference
  QVariant resolve(const QString &id, const QString &type) override;

protected:
  //! The level type
  SongAssetSet *mType;
};

}