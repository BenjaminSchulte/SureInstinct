#include <aoba/log/Log.hpp>
#include <aoba/tileset/TileFlagGroupDefaultGenerator.hpp>

using namespace Aoba;

// -----------------------------------------------------------------------------
bool TileFlagGroupDefaultGenerator::generate(const TileFlagGroupTile &tile, uint64_t &value) const {
  uint64_t orValue = 0;

  if (!mConfig.numericParameter.isEmpty()) {
    auto *par = mGroup->parameter(mConfig.numericParameter);
    if (!par) {
      Aoba::log::error("Undefined parameter: " + mConfig.numericParameter);
      return false;
    }

    orValue |= par->numericValue(tile.values[mConfig.numericParameter]);
  }

  value |= orValue;

  return true;
}