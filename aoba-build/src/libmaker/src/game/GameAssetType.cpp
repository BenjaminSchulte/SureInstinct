#include <maker/game/GameAsset.hpp>
#include <maker/game/GameLinkResult.hpp>

using namespace Maker;
using namespace Aoba;

// -----------------------------------------------------------------------------
bool AbstractGameAssetType::load(Aoba::AbstractAsset *asset) const {
  return asset->load();
}

// -----------------------------------------------------------------------------
bool AbstractGameAssetType::build(Aoba::AbstractAsset *asset) const {
  return asset->build();
}

// -----------------------------------------------------------------------------
bool AbstractGameAssetType::link(Aoba::AbstractAsset *asset, GameLinkResult &result) const {
  for (const QString &obj : asset->getFmaObjectFiles()) {
    result.addObject(obj);
  }

  QString code = asset->getFmaCode();
  if (!code.isEmpty()) {
    result.addCode(code);
  }

  return true;
}

// -----------------------------------------------------------------------------
