#pragma once

#include "../script/ScriptAssetResolver.hpp"

namespace Aoba {
class LevelAssetSet;

class LevelScriptAssetResolver : public ScriptAssetResolver {
public:
  //! Constructor
  LevelScriptAssetResolver(LevelAssetSet *type) : mType(type) {}

  //! Returns a reference
  QVariant resolve(const QString &id, const QString &type) override;

protected:
  //! The level type
  LevelAssetSet *mType;
};

}