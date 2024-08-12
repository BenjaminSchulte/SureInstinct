#pragma once

#include "../script/ScriptAssetResolver.hpp"

namespace Aoba {
class CharacterAssetSet;
class Character;

class CharacterScriptAssetResolver : public ScriptAssetResolver {
public:
  //! Constructor
  CharacterScriptAssetResolver(CharacterAssetSet *type) : mType(type) {}

  //! Returns a reference
  QVariant resolve(const QString &id, const QString &type) override;

protected:
  //! Resolve sprite references
  QVariant resolveSprite(Character *character, const QString &id, const QString &type);

  //! The Character type
  CharacterAssetSet *mType;
};

}