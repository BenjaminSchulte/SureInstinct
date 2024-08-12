#include <aoba/sprite/SpriteType.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
SpriteType::SpriteType(SpriteTypeAssetSet *set, const QString &id)
  : Asset(set, id)
{
  set->add(this);
}
