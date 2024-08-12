#pragma once

#include "../script/ScriptAssetResolver.hpp"

namespace Aoba {
class StreamAssetSet;

class StreamScriptAssetResolver : public ScriptAssetResolver {
public:
  //! Constructor
  StreamScriptAssetResolver(StreamAssetSet *type) : mType(type) {}

  //! Returns a reference
  QVariant resolve(const QString &id, const QString &type) override;

protected:
  //! The level type
  StreamAssetSet *mType;
};

}